#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

#define PI 3.141592f

#define MAP_WIDTH 16000.0f
#define MAP_HEIGHT 9000.0f

#define POD_NB 2
#define POD_MAXIMUM_THRUST 100
#define POD_COLLIDER_SIZE 400.0f

#define CHECKPOINT_MAXIMUM_NB 8
#define CHECKPOINT_RADIUS 600.0f

#define BOOST_THRESHOLD_DISTANCE_TO_CHECKPOINT 1500.0f
#define BOOST_THRESHOLD_DISTANCE_TO_OPPONENT 600.0f
#define BOOST_THRESHOLD_DOT_OPPONENT 0.90f
#define BOOST_THRESHOLD_ANGLE 1.0f
#define BOOST_KEYWORD "BOOST"

#define THRUST_ANGLE_BEFORE_TURNING 180.0f 
#define THRUST_DISTANCE_BEFORE_BRAKING 1200.0f
#define THRUST_MINIMUM_BRACKING_MULTIPLIER 0.5f

#define FLOAT_COMPARE(_a, _b) (fabs(_a - _b) < 0.000001f)
#define DEG_TO_RAD(angleDeg) (angleDeg * PI / 180.0f)
#define RAD_TO_DEG(angleRad) (angleRad * 180.0f / PI)

#pragma region Vector2

class Vector2
{
public:

	static const Vector2 Zero;
	static const Vector2 Up;
	static const Vector2 Right;

	Vector2() = default;
	Vector2(float _x, float _y = 0.0f) : m_x(_x), m_y(_y) {};
	Vector2(const Vector2& _other) : m_x(_other.m_x), m_y(_other.m_y) {};

	float m_x = 0;
	float m_y = 0.;

	static float Dot(const Vector2& _v1, const Vector2& _v2);
	static float Distance(const Vector2& _a, const Vector2& _b);
	static float SquareDistance(const Vector2& _a, const Vector2& _b);

	float Magnitude() const;
	inline float SquareMagnitude() const;
	Vector2 Normalized() const;

	inline bool operator== (const Vector2& _v) const { return (FLOAT_COMPARE(m_x, _v.m_x) && FLOAT_COMPARE(m_y, _v.m_y)); };
	inline bool operator!= (const Vector2& _v) const { return (!FLOAT_COMPARE(m_x, _v.m_x) || !FLOAT_COMPARE(m_y, _v.m_y)); };
	inline Vector2& operator+= (const Vector2& _v);
	inline Vector2& operator-= (const Vector2& _v);
	inline Vector2& operator*= (float _multiplier);
	inline Vector2& operator/= (float _divider);
	inline Vector2 operator-() const;
	inline Vector2 operator+ (const Vector2& _v) const;
	inline Vector2 operator- (const Vector2& _v) const;
	inline Vector2 operator* (float _multiplier) const;
	inline Vector2 operator/ (float _divider) const;

	friend ostream& operator<< (ostream& _o, const Vector2& _v);
};

const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::Up = Vector2(0.0f, -1.0f);
const Vector2 Vector2::Right = Vector2(1.0f, 0.0f);

float Vector2::Dot(const Vector2& _v1, const Vector2& _v2)
{
	if (_v1 == Zero || _v2 == Zero) { return 0.0f; }
	return (_v1.m_x * _v2.m_x) + (_v1.m_y * _v2.m_y);
}

float Vector2::Distance(const Vector2& _a, const Vector2& _b)
{
	return sqrtf(SquareDistance(_a, _b));
}

float Vector2::SquareDistance(const Vector2& _a, const Vector2& _b)
{
	if (_a == _b) return 0.0f;
	float distanceX = abs(_b.m_x - _a.m_x);
	float distanceY = abs(_b.m_y - _a.m_y);
	return (distanceX * distanceX) + (distanceY * distanceY);
}

float Vector2::Magnitude() const
{
	if (m_x == 0.0f && m_y == 0.0f) return 0.0f;

	float magnitude = SquareMagnitude();
	return sqrtf(magnitude);
}

float Vector2::SquareMagnitude() const
{
	return (m_x * m_x) + (m_y * m_y);
}

Vector2 Vector2::Normalized() const
{
	if (*this == Zero) return Zero;
	if (FLOAT_COMPARE(m_x, 0.0f)) return Vector2(0.0f, 1.0f);
	if (FLOAT_COMPARE(m_y, 0.0f)) return Vector2(1.0f, 0.0f);

	float magnitude = this->Magnitude();
	Vector2 vecToReturn = Vector2((m_x / magnitude), (m_y / magnitude));
	return vecToReturn;
}

Vector2& Vector2::operator+=(const Vector2& _v)
{
	m_x += _v.m_x;
	m_y += _v.m_y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& _v)
{
	m_x -= _v.m_x;
	m_y -= _v.m_y;
	return *this;
}

Vector2& Vector2::operator*=(float _multiplier)
{
	m_x *= _multiplier;
	m_y *= _multiplier;
	return *this;
}

Vector2& Vector2::operator/=(float _divider)
{
	m_x /= _divider;
	m_y /= _divider;
	return *this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-m_x, -m_y);
}

Vector2 Vector2::operator+(const Vector2& _v) const
{
	Vector2 newVec = Vector2(*this);
	newVec += _v;
	return newVec;
}

Vector2 Vector2::operator-(const Vector2& _v) const
{
	Vector2 newVec = Vector2(*this);
	newVec -= _v;
	return newVec;
}
Vector2 Vector2::operator*(float _multiplier) const
{
	Vector2 newVec = Vector2(*this);
	newVec *= _multiplier;
	return newVec;
}

Vector2 Vector2::operator/(float _divider) const
{
	Vector2 newVec = Vector2(*this);
	newVec /= _divider;
	return newVec;
}

ostream& operator<<(ostream& _output, const Vector2& _v)
{
	_output << (int)_v.m_x << " " << (int)_v.m_y;
	return _output;
}

#pragma endregion

#pragma region Checkpoint

class Checkpoint
{
public:

	static void CheckGreatestDistance(Checkpoint _checkpoints[], int _nbCheckpoints);

	void ReceiveInput();
	void CalculateValuesToNextCheckpoint(const Checkpoint& _nextCheckpoint);

	bool m_haveGreatestDistance = false;
	Vector2 m_position = Vector2::Zero; // Position of the next check point

private:

	float m_distanceToNextCheckpoint = 0.0f; // Distance to the next checkpoint
};

void Checkpoint::CheckGreatestDistance(Checkpoint _checkpoints[], int _nbCheckpoints)
{
	float greatestDistance = 0.0f;
	Checkpoint* greatestDistanceCheckpoint = nullptr;

	for (size_t iCheckpoint = 0; iCheckpoint < _nbCheckpoints; iCheckpoint++)
	{
		if (_checkpoints[iCheckpoint].m_distanceToNextCheckpoint > greatestDistance)
		{
			greatestDistance = _checkpoints[iCheckpoint].m_distanceToNextCheckpoint;
			greatestDistanceCheckpoint = &_checkpoints[iCheckpoint];
		}
	}
	if (greatestDistanceCheckpoint != nullptr) greatestDistanceCheckpoint->m_haveGreatestDistance = true;
}

void Checkpoint::ReceiveInput()
{
	cin >> m_position.m_x >> m_position.m_y;
	cin.ignore();
}

void Checkpoint::CalculateValuesToNextCheckpoint(const Checkpoint& _nextCheckpoint)
{
	Vector2 checkpointToNext = _nextCheckpoint.m_position - m_position;
	m_distanceToNextCheckpoint = checkpointToNext.Magnitude();
}

#pragma endregion

#pragma region Pod

class Pod
{
public:

	void ReceiveInput(int _index);
	inline Vector2 GetPosition() const { return m_position; }
	void UpdateCurrentCheckpoint(Checkpoint _checkpoints[]);

protected:

	int m_index = 0;
	Vector2 m_position = Vector2::Zero;
	Vector2 m_speed = Vector2::Zero;
	float m_angle = 0.0f;
	bool m_canUseBoost = true;
	int m_currentCheckpointID = 1;
	Checkpoint* m_currentCheckpoint = nullptr;
	float m_currentCheckpointAngle = 0.0f;
	float m_currentCheckpointDistance = 0.0f;
};

void Pod::ReceiveInput(int _index)
{
	m_index = _index;

	cin >> m_position.m_x >> m_position.m_y
		>> m_speed.m_x >> m_speed.m_y
		>> m_angle
		>> m_currentCheckpointID;
	cin.ignore();
}

void Pod::UpdateCurrentCheckpoint(Checkpoint _checkpoints[])
{
	m_currentCheckpoint = &_checkpoints[m_currentCheckpointID];
	cerr << "Checkpoint index: " << m_currentCheckpointID << endl;

	Vector2 checkpointDirection = m_currentCheckpoint->m_position - m_position;
	Vector2 checkpointDirectionNormalized = checkpointDirection.Normalized();

	m_currentCheckpointAngle = RAD_TO_DEG(acosf(checkpointDirectionNormalized.m_x));
	if (checkpointDirectionNormalized.m_y < 0.0f)
	{
		m_currentCheckpointAngle = 360.0f - m_currentCheckpointAngle;
	}
	m_currentCheckpointAngle = abs(m_currentCheckpointAngle - m_angle);

	cerr << "Angle to checkpoint: " << m_currentCheckpointAngle << endl;
	cerr << "Angle of pod: " << m_angle << endl;

	m_currentCheckpointDistance = checkpointDirection.Magnitude();
	cerr << "Distance to checkpoint: " << m_currentCheckpointDistance << endl;
}

#pragma endregion

#pragma region PlayerPod

class PlayerPod : public Pod
{
public:
	void UpdateOpponentInteraction(Pod _opponents[]);
	void UpdateThrust();
	void UpdateTarget();
	void UpdateHoverText();

	void SendOutput();

private:

	bool CheckShouldBoost() const;

	int m_thrust = POD_MAXIMUM_THRUST;
	string m_hoverText = "";
	Vector2 m_target = Vector2::Zero;

	Vector2 m_playerToOpponent[POD_NB] = { Vector2::Zero , Vector2::Zero };
	float m_dotOpponentPlayer[POD_NB] = { 0.0f, 0.0f };

	bool m_isTurning = false;
	bool m_isBraking = false;
};

void PlayerPod::UpdateOpponentInteraction(Pod _opponents[])
{
	for (size_t i = 0; i < POD_NB; i++)
	{
		Pod opponent = _opponents[i];
		// Calculate the dot product of the opponent and the player
		Vector2 m_playerToTarget = m_target - m_position;
		m_playerToOpponent[i] = opponent.GetPosition() - m_position;
		m_dotOpponentPlayer[i] = Vector2::Dot(m_playerToTarget.Normalized(), m_playerToOpponent[i].Normalized());
		cerr << "Dot between opponent and player : " << m_dotOpponentPlayer << endl;
	}
}

void PlayerPod::UpdateThrust()
{
	float newThrust = POD_MAXIMUM_THRUST;

	float turningMultiplier = 1.0f;
	bool isTurning = false;

	float brakingMultiplier = 1.0f;
	bool isBraking = false;

	// Slow down when the player is turning to the next checkpoint
	turningMultiplier = (1.0f - m_currentCheckpointAngle / THRUST_ANGLE_BEFORE_TURNING);
	turningMultiplier = clamp(turningMultiplier, 0.0f, 1.0f);
	cerr << "Turning Multiplier : " << turningMultiplier << endl;
	m_isTurning = turningMultiplier <= 0.90f;

	// Slow down as the player get closer to the checkpoint.
	brakingMultiplier = m_currentCheckpointDistance / THRUST_DISTANCE_BEFORE_BRAKING;
	brakingMultiplier = clamp(brakingMultiplier, THRUST_MINIMUM_BRACKING_MULTIPLIER, 1.0f);
	cerr << "Braking Multiplier : " << brakingMultiplier << endl;
	m_isBraking = brakingMultiplier <= 0.90f;

	m_thrust = (int)(newThrust * turningMultiplier * brakingMultiplier);
	cerr << "Thrust : " << m_thrust << endl;
}

void PlayerPod::UpdateTarget()
{
	// Prevent the player from going in the wrong direction at the start
	static bool isFirstFrame = true;
	if (isFirstFrame)
	{
		isFirstFrame = false;
		m_target = m_currentCheckpoint->m_position;
		return;
	}

	// Create an offset to the checkpoint position, taking into account of the player speed
	Vector2 offset = m_speed * -3.0f;
	m_target = m_currentCheckpoint->m_position + offset;
}

void PlayerPod::UpdateHoverText()
{
	if (m_isBraking && m_isTurning)
	{
		m_hoverText = "TURNING & BRAKING";
	}
	else if (m_isBraking)
	{
		m_hoverText = "BRAKING";
	}
	else if (m_isTurning)
	{
		m_hoverText = "TURNING";
	}
	else
	{
		m_hoverText = "ACCELERATING";
	}
	m_hoverText = m_hoverText + " TO CP " + std::to_string(m_currentCheckpointID);
}

void PlayerPod::SendOutput()
{
	if (CheckShouldBoost() == true)
	{
		m_canUseBoost = false;
		cout << m_target << " " << BOOST_KEYWORD << " " << m_hoverText << endl;
	}
	else
	{
		cout << m_target << " " << m_thrust << " " << m_hoverText << endl;
	}
}

bool PlayerPod::CheckShouldBoost() const
{
	if (false == m_canUseBoost) return false;

	if (false == m_currentCheckpoint->m_haveGreatestDistance && m_index != 0) return false;

	bool hasGoodAngleToCheckpoint = m_currentCheckpointAngle < BOOST_THRESHOLD_ANGLE;
	if (false == hasGoodAngleToCheckpoint) return false;

	bool hasGoodDistanceToCheckpoint = m_currentCheckpointDistance > BOOST_THRESHOLD_DISTANCE_TO_CHECKPOINT;
	if (false == hasGoodDistanceToCheckpoint) return false;

	for (size_t i = 0; i < POD_NB; i++)
	{
		bool isInFrontOfOpponent = (m_dotOpponentPlayer[i] > BOOST_THRESHOLD_DOT_OPPONENT);
		if (isInFrontOfOpponent)
		{
			bool hasGoodDistanceToOpponent = m_playerToOpponent[i].Magnitude() < BOOST_THRESHOLD_DISTANCE_TO_OPPONENT;
			if (false == hasGoodDistanceToOpponent) { return false; }
		}
	}

	return true;
}

#pragma endregion

int main()
{
	Pod opponents[POD_NB];
	PlayerPod playerPods[POD_NB];
	Checkpoint checkpoints[CHECKPOINT_MAXIMUM_NB];

	int laps;
	cin >> laps; cin.ignore();

	int checkpointCount;
	cin >> checkpointCount;
	cin.ignore();

	for (size_t iCheckpoint = 0; iCheckpoint < checkpointCount; iCheckpoint++)
	{
		checkpoints[iCheckpoint].ReceiveInput();
		int iPpreviousCheckpoint = iCheckpoint - 1;
		if (iCheckpoint > 0) checkpoints[checkpointCount - 1].CalculateValuesToNextCheckpoint(checkpoints[iCheckpoint]);
		if (iCheckpoint == checkpointCount - 1) checkpoints[iCheckpoint].CalculateValuesToNextCheckpoint(checkpoints[0]);
	}
	Checkpoint::CheckGreatestDistance(checkpoints, checkpointCount);

	bool isRunning = true;
	while (isRunning)
	{
		for (size_t iPod = 0; iPod < POD_NB; iPod++)
		{
			playerPods[iPod].ReceiveInput(iPod);
		}
		for (size_t iPod = 0; iPod < POD_NB; iPod++)
		{
			opponents[iPod].ReceiveInput(iPod);
		}

		for (size_t iPod = 0; iPod < POD_NB; iPod++)
		{
			cerr << "------- POD 0" << iPod + 1 << " -------" << endl;
			playerPods[iPod].UpdateCurrentCheckpoint(checkpoints);
			playerPods[iPod].UpdateTarget();
			playerPods[iPod].UpdateThrust();
			playerPods[iPod].UpdateOpponentInteraction(opponents);
			playerPods[iPod].UpdateHoverText();
			cerr << endl;
		}

		for (size_t iPod = 0; iPod < POD_NB; iPod++)
		{
			playerPods[iPod].SendOutput();
		}
	}
}
