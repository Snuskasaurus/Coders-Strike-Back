#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

#define BOOST_THRESHOLD_DISTANCE 8500
#define BOOST_THRESHOLD_ANGLE 1
#define BOOST_KEYWORD "BOOST"

#pragma region Checkpoint

struct Checkpoint
{
	int m_x = 0; // x position of the next check point
	int m_y = 0; // y position of the next check point
	int m_distance = 0; // distance to the next checkpoint
	int m_angle = 0; // angle in degres (from -180 to 180) between your pod orientation and the direction of the next checkpoint
};

#pragma endregion

#pragma region Pod

class Pod
{
public:
	virtual void ManageInput();
	virtual void ManageDebug() {};

protected:
	int m_x = 0;
	int m_y = 0;
	int m_thrust = 100;
	bool m_canUseBoost = true;
};

void Pod::ManageInput()
{
	cin >> m_x >> m_y;
	cin.ignore();
}

#pragma endregion

#pragma region PlayerPod

class PlayerPod : public Pod
{
public:
	void ManageInput() override;
	void ManageDebug() override;
	void ManageOutput();
private:
	int ComputeThrust();
	bool CheckShouldBoost();

	Checkpoint m_nextCheckpoint;
};

void PlayerPod::ManageInput()
{
	cin >> m_x >> m_y >>
		m_nextCheckpoint.m_x >> m_nextCheckpoint.m_y >>
		m_nextCheckpoint.m_distance >> m_nextCheckpoint.m_angle;
	cin.ignore();
}

void PlayerPod::ManageDebug()
{
	cerr << "Thrust : " << m_thrust << endl;
	cerr << "Can use boost : " << m_canUseBoost << endl;
	cerr << "Distance to checkpoint : " << m_nextCheckpoint.m_distance << endl;
	cerr << "Angle to checkpoint : " << m_nextCheckpoint.m_angle << endl;
}

void PlayerPod::ManageOutput()
{
	if (CheckShouldBoost())
	{
		cout << m_nextCheckpoint.m_x << " " << m_nextCheckpoint.m_y << " " << BOOST_KEYWORD << endl;
	}
	else
	{
		m_thrust = ComputeThrust();
		cout << m_nextCheckpoint.m_x << " " << m_nextCheckpoint.m_y << " " << m_thrust << endl;
	}
}

int PlayerPod::ComputeThrust() 
{
	int thrust = 100;

	if (m_nextCheckpoint.m_angle > 90 or m_nextCheckpoint.m_angle < -90)
	{
		thrust = 0;
	}
	else
	{
		thrust = 100;
	}
	return thrust;
}

bool PlayerPod::CheckShouldBoost()
{
	if (false == m_canUseBoost) return false;

	bool hasGoodAngle = abs(m_nextCheckpoint.m_angle) < BOOST_THRESHOLD_ANGLE;
	if (false == hasGoodAngle) return false;

	bool hasGoodDistance = m_nextCheckpoint.m_distance < BOOST_THRESHOLD_DISTANCE;
	if (false == hasGoodDistance) return false;

	m_canUseBoost = false;
	return true;
}

#pragma endregion

int main()
{
	PlayerPod playerPod;
	Pod opponent;

	// Game loop
	bool isRunning = true;
	while (isRunning)
	{
		playerPod.ManageInput();
		opponent.ManageInput();
		playerPod.ManageDebug();
		playerPod.ManageOutput();
	}
}