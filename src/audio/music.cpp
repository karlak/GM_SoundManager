#include <iostream>
#include <string>
#include <sndfile.h>
#include <soxr.h>
#include <fstream>
#include <sstream>
#include "music.h"
#include "soundmanager.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <chrono>
#include <thread>

void throwError(std::string s){
    std::cout << "ERROR: " << s << std::endl;
    throw s;
}




void Music::writeData(float *out, unsigned long frameCount, SoundManager *data)
{
    if (this->loaded) {

		if (this->order_stop) {
			this->paused = true;
			if (this->paused_gain < PAUSE_FADE_LIMIT || this->position >= this->nbSamples) {
				this->stopped_position = this->position;
				this->position = 0;
				this->order_stop = false;
			}
		}

		if (this->order_seek != -1) {
			this->position = this->order_seek;
			if (this->loadedInfiniteLooper)
				this->looperInfos->exit_jump();
			this->order_seek = -1;
		}

		/////////////
		if (this->position >= this->nbSamples) {
			gain = desired_gain;
			return;
		}
		for (unsigned int i = 0; i < frameCount; i++) {
			update_gain();
			update_balance();

			if (this->paused) {
				if (this->paused_gain < PAUSE_FADE_LIMIT) {
					return;
				}
				this->paused_gain *= PAUSE_FADE_RATE;
			}
			else {
				this->paused_gain = std::min(1.f, this->paused_gain / PAUSE_FADE_RATE);
			}
            

			float* dataPtr = &this->data[this->position];

			if (this->loadedInfiniteLooper) {
				LooperInfos* looper = this->looperInfos;
				LooperElement* e;
				if (e = looper->get_jump(position / nbChannels)) {
					static float dataLooper[64];
					for (int i = 0; i < nbChannels; i++) {
						auto jump_pos = looper->current_jump_progression;
						dataLooper[i] = 0;
						/*long from = e->sampleFrom;
						long to = e->sampleTo;
						long chan = nbChannels;
						long res = (e->sampleFrom + jump_pos)*nbChannels + i;
						*/
						dataLooper[i] += this->data[(e->sampleFrom + jump_pos)*nbChannels + i] * ((e->length - jump_pos) / (float)e->length);
						dataLooper[i] += this->data[(e->sampleTo + jump_pos)*nbChannels + i] * ((jump_pos) / (float)e->length);
						
						// This fator prevent a bit of the loss of volume from the mixing of the chunks (=1 at the beg/end; =1.25 at the middle). 
						// Those values seem to work
						float factor = (-(((jump_pos) / (float)e->length) - 0.5)*(((jump_pos) / (float)e->length) - 0.5) + 0.25) + 1;
						dataLooper[i] *= factor;
					}
					looper->current_jump_progression++;
					dataPtr = dataLooper;
					if (looper->current_jump_progression == e->length) {
						this->position = (e->sampleTo + looper->current_jump_progression - 1)*nbChannels;
					}
				}
			}


			if (this->nbChannels == 1) {
				//left
				*out++ += *dataPtr * this->gain_log * this->paused_gain * this->balance_left_gain * data->global_gain;
				//right
				*out++ += *dataPtr * this->gain_log * this->paused_gain * this->balance_right_gain * data->global_gain;
				this->position++;
			}
			else {
				//left
				*out++ += *dataPtr * this->gain_log * this->paused_gain * this->balance_left_gain * data->global_gain;
				this->position++;
				dataPtr++;
				//right
				*out++ += *dataPtr * this->gain_log * this->paused_gain * this->balance_right_gain * data->global_gain;
				this->position += this->nbChannels - 1;
			}

			if (this->position >= this->nbSamples) {
                this->position = 0;
                if (!this->repeat){
                    this->paused = true;
                    this->paused_gain = PAUSE_FADE_LIMIT * 0.75;
					return;
				}
			}
		}

    }
}

void Music::update_gain()
{
    if (paused_gain < PAUSE_FADE_LIMIT)
		gain = desired_gain;
	else
		gain += (desired_gain - gain) * UPDATE_GAIN_RATE;
    
    //y = (Math.exp(x)-1)/(Math.E-1)
    gain_log = (powf(10, gain)-1)/9;
    
}

void Music::update_balance()
{
    if (paused_gain < PAUSE_FADE_LIMIT)
		balance = desired_balance;
	else
		balance += (desired_balance - balance) * UPDATE_BALANCE_RATE;

	double angle = balance * PI_4;
	balance_left_gain = (float)(SQRT2_2 * (cos(angle) - sin(angle)));
	balance_right_gain = (float)(SQRT2_2 * (cos(angle) + sin(angle)));
}


Music::Music() {
    loaded = false;
    repeat = false;
    desired_gain = gain = 0.3333333f;
    position = 0;
    order_seek = -1;
    order_stop = false;
    paused = true;
    paused_gain = 1;
    balance = 0;
    desired_balance = 0;
    loadedInfiniteLooper = false;
    nbChannels = 1;
}

Music::~Music()
{
    if (loaded) unload();
}

void Music::load(const char *filename)
{
    if (loaded) {
        unload();
    }
    
    repeat = false;
    gain = 0.3333333f;
    position = 0;
    order_seek = -1;
    order_stop = false;
    paused = true;
    paused_gain = 0.0001f;
    loadedInfiniteLooper = false;
    
    SNDFILE* file;
    SF_INFO sfinfo;
    
    file = sf_open(filename, SFM_READ, &sfinfo);
    if (!file) {
        throwError(std::string("Impossible d'ouvrir le fichier. ")+std::string(filename));
    }
    
    nbSamples = sfinfo.channels * (long)sfinfo.frames;
    sampleRate_file = sfinfo.samplerate;
    sampleRate = sfinfo.samplerate;
    nbChannels = sfinfo.channels;
    
    data = new float[nbSamples];
    if (sf_read_float(file, data, nbSamples) != nbSamples) {
        throwError("Samples error.");
    }
    sf_close(file);
    
    if (SAMPLE_RATE != sampleRate) {
        throwError(std::string("Wrong SampleRate ! Given:")+std::to_string(sampleRate)+". Expected: "+std::to_string(SAMPLE_RATE));
        double irate = sampleRate;
        double orate = SAMPLE_RATE;
        size_t olen = (size_t)(sfinfo.frames*(orate / irate) + 0.5f);
        float* out = new float[olen*nbChannels];
        size_t odone;
        soxr_runtime_spec_t runtime = soxr_runtime_spec(4);
        soxr_quality_spec_t quality = soxr_quality_spec(SOXR_QQ, 0);
        
        soxr_error_t error = soxr_oneshot(irate, orate, nbChannels,
                                          data, (long)sfinfo.frames, NULL,
                                          out, olen, &odone,
                                          NULL,
                                          &quality,
                                          &runtime);
        if(error)
            throwError(std::string(soxr_strerror(error)));
        delete[] data;
        data = out;
        nbSamples = odone * nbChannels;
        sampleRate = SAMPLE_RATE;
    }
    loaded = true;
}

void Music::loadInfiniteLooper(const char *filename)
{
    try {
		if (loadedInfiniteLooper)
			unloadInfiniteLooper();
		looperInfos = new LooperInfos(filename);
		loadedInfiniteLooper = true;
	}
	catch (std::exception &e) {
		throwError(e.what());
	}
}

void Music::unload()
{
    if (loaded) {
        stop(true);
        loaded = false;
        delete[] data;
    }
}

void Music::unloadInfiniteLooper()
{
    if (loadedInfiniteLooper) {
		stop(true);
		loadedInfiniteLooper = false;
		delete looperInfos;
    }
}

void Music::stop(bool wait)
{
    if (!loaded)
		return;
	order_stop = 1;
	if (wait) {
		while (order_stop)
			std::this_thread::sleep_for(std::chrono::milliseconds(10)); // @check if it works
    }
}

void Music::setPaused(bool pause)
{
    if(this->paused != pause) paused_gain = pause ? 1.f : 0.002f; this->paused = pause; 
}

LooperInfos::LooperInfos(const char *filename)
{
    current_jump = nullptr;
	current_jump_progression = 0;

	std::ifstream file(filename);
	if (!file.good()) {
		throwError("Impossible d'ouvrir le fichier boucle.");
	}
	std::string line;

	try {
		int i;
		for (i = 0; std::getline(file, line) /*&& i<50*/;)   //read stream line by line
		{
			std::istringstream in(line);      //make a stream for the line itself
			float score = 0;
			long s_to = 0;
			long s_from = 0;
			long length = 0;
			float t1 = 0;
			float t2 = 0;
			int activatedBack = 0;
			int activatedForward = 0;

			in >> score >> s_to >> s_from >> length >> t1 >> t2 >> activatedBack >> activatedForward;

			if (activatedBack) {
				elems.resize(i + 1);
				elems[i].score = score;
				elems[i].sampleTo = s_to;
				elems[i].sampleFrom = s_from;
				elems[i].length = length;
				elems[i].time1 = t1;
				elems[i].time2 = t2;
				if (activatedBack == 2) {
					elems[i].last = true;
					last_jump_position = elems[i].sampleFrom;
				}
				else
					elems[i].last = false;
				map_sample_from_index[elems[i].sampleFrom].push_back(i);
				i++;
			}
			if (activatedForward && activatedBack!=2) {
				elems.resize(i + 1);
				elems[i].score = score;
				elems[i].sampleTo = s_from;
				elems[i].sampleFrom = s_to;
				elems[i].length = length;
				elems[i].time1 = t2;
				elems[i].time2 = t1;
				elems[i].last = false;
				map_sample_from_index[elems[i].sampleFrom].push_back(i);
				i++;
			}
		}
	}
	catch (std::exception &e) {
		throwError(e.what());
    }
}

LooperElement *LooperInfos::get_jump(long current_sample)
{
    if (current_jump) {
		if (current_jump_progression < current_jump->length) {
			return current_jump;
		}
		else {
			current_jump = nullptr;
			current_jump_progression = 0;
		}
	}
	LooperElement* e = nullptr;

	auto const it = map_sample_from_index.find(current_sample);
	if (it != map_sample_from_index.end()) {
		auto vec = it->second;
		auto l = vec.size();
		auto index = vec[rand()%l];
		e = &elems[index];
		if (e->last) {
			current_jump = e;
		}
		else {
			if (rand() % 100 < 20) {
				current_jump = e;
			}
		}
	}

	if (current_jump) {
		if (current_jump->sampleTo + current_jump->length >= last_jump_position)
			current_jump = nullptr;
	}
	
	return current_jump;
}

void LooperInfos::exit_jump() { current_jump = NULL; }
