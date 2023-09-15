#pragma once

#include "pch.hpp"

struct playback_cmd
{
	int serverTime;
	int angles[3];
	int32_t buttons;
	char forwardmove;
	char rightmove;
	int FPS;
	char weapon;
	char offhand;
	fvec3 viewangles;
	fvec3 origin;
	fvec3 velocity;
	fvec3 mins, maxs;
	float camera_yaw;

};


struct recorder_cmd
{
	int serverTime;
	int angles[3]; //delta angles
	int32_t buttons;
	char forwardmove;
	char rightmove;
	int FPS;
	char weapon;
	char offhand;
	float viewangles[3];
	fvec3 origin;
	fvec3 velocity;
};

class Playback
{
public:
	static Playback& getInstance() {
		static Playback instance;
		return instance;
	}
	//void Record(usercmd_s* cmd, const int FPS) noexcept;
	//void StopRecording() noexcept { recording = false; }

	void ExternalPlayback(const std::list<playback_cmd>& cmds) { 
		clear();
		
		for (auto& i : cmds) 
			recorder_sequence.push_back(i); 

		StopPlayback();

	}
	void StartPlayback() noexcept;
	void StartRecording(int* angles) noexcept { VectorCopy(angles, recordingAngle);  recording = true; state = *ps_loc; }
	bool isRecording() const noexcept { return recording; }

	void doPlayback(usercmd_s* cmd) noexcept;
	void StopPlayback() noexcept { it = recorder_sequence.begin(); playback = false; }
	bool IsPlayback() const noexcept { return playback; }

	void clear() noexcept { recorder_sequence.clear(); playback = false; }
	decltype(auto) getIterator() const { return std::ref(it); }


	void DrawPlayback();
	playerState_s state;

private:
	std::list<playback_cmd> recorder_sequence;
	int frame = 0;
	bool refresh_start_time = true;

	void iterateIterator(const size_t amount) { return refresh_start_time = true, std::advance(it, amount); }
	playback_cmd* CurrentCmd() const;

	bool playback = false;
	bool recording = false;
	std::list<playback_cmd>::iterator it;
	int recordingAngle[3]{};
	int refTime = 0;

};