#pragma once

#include "pch.hpp"

struct prediction_state
{
	pmove_t pm;
	pml_t pml;
	playerState_s ps;
};


enum class prediction_angle_enumerator
{
	FIXED_TURN,
	FIXED_ANGLE,
	STRAFEBOT,

};

class prediction_viewangle
{
public:
	prediction_viewangle() = default;
	virtual prediction_angle_enumerator get_type() const = 0;
	virtual fvec3 get_deltas(pmove_t* pm, pml_t* pml, struct prediction_controller* controller) const = 0;
 };

struct prediction_controller
{
	prediction_controller() = default;
	char forwardmove = 127;
	char rightmove = 0;
	int32_t numInputs = 100;
	int buttons = 0;
	int FPS = 125;
	fvec3 viewangles;
	std::unique_ptr<prediction_viewangle> turntype;
	prediction_angle_enumerator turntype_enum = prediction_angle_enumerator::FIXED_TURN;

	prediction_controller(const prediction_controller&) = delete;
	prediction_controller& operator=(const prediction_controller&) = delete;
};

class prediction_viewangle_fixed_turn : public prediction_viewangle
{
public:
	prediction_angle_enumerator get_type() const override 
	{
		return prediction_angle_enumerator::FIXED_TURN;
	}
	fvec3 get_deltas(pmove_t* pm, pml_t* pml, struct prediction_controller* controller) const override
	{
		return {up, -right, 0.f};
	}

	float right;
	float up;
};

struct simulation_results
{
	ivec3 cmd_angles;
	char weapon;
	char offhand;
	fvec3 viewangles;
	fvec3 origin;
	fvec3 velocity;
	fvec3 mins, maxs;
	float camera_yaw = 0;
};

simulation_results PmoveSingleSimulation(pmove_t* pm, pml_t* pml, prediction_controller* controller);