#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

#define PI 3.141592f

#define MAP_WIDTH 16000.0f
#define MAP_HEIGHT 9000.0f

#define CHECKPOINT_RADIUS 600.0f

#define POD_MAXIMUM_THRUST 100
#define POD_COLLIDER_SIZE 400.0f

#define BOOST_THRESHOLD_DISTANCE_TO_CHECKPOINT 1500.0f
#define BOOST_THRESHOLD_DISTANCE_TO_OPPONENT 600.0f
#define BOOST_THRESHOLD_DOT_OPPONENT 0.90f
#define BOOST_THRESHOLD_ANGLE 1.0f
#define BOOST_KEYWORD "BOOST"

#define THRUST_ANGLE_BEFORE_TURNING 80.0f 
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

	Vector2 m_position = Vector2::Zero; // position of the next check point
	float m_distance = 0.0f; // distance to the next checkpoint
	float m_angle = 0.0f; // angle in degres (from -180 to 180) between your pod orientation and the direction of the next checkpoint
};


#pragma endregion

#pragma region Pod

class Pod
{
public:

	virtual void ReceiveInput();
	virtual void UpdateDebug() {};

	inline Vector2 GetPosition() const { return m_position; }

protected:

	Vector2 m_position = Vector2::Zero;
	bool m_canUseBoost = true;
};

void Pod::ReceiveInput()
{
	cin >> m_position.m_x >> m_position.m_y;
	cin.ignore();
}

#pragma endregion

#pragma region PlayerPod

class PlayerPod : public Pod
{
public:

	void ReceiveInput() override;

	void UpdateOpponentInteraction(const Pod& _opponent);
	void UpdateThrust();
	void UpdateSpeed();
	void UpdateNextCheckpoint();
	void UpdateTarget();
	void UpdateHoverText();

	void SendOutput();

private:

	bool CheckShouldBoost();

	Checkpoint m_nextCheckpoint;
	int m_thrust = POD_MAXIMUM_THRUST;
	Vector2 m_playerToTarget = Vector2::Zero;
	Vector2 m_playerToOpponent = Vector2::Zero;
	float m_dotOpponentPlayer = 0.0f;
	string m_hoverText = "";
	Vector2 m_lastPosition = Vector2::Zero;
	Vector2 m_speed = Vector2::Zero;
	Vector2 m_target = Vector2::Zero;

	bool m_isTurning = false;
	bool m_isBraking = false;
};

void PlayerPod::ReceiveInput()
{
	cin >> m_position.m_x >> m_position.m_y >>
		m_nextCheckpoint.m_position.m_x >> m_nextCheckpoint.m_position.m_y >>
		m_nextCheckpoint.m_distance >> m_nextCheckpoint.m_angle;
	cin.ignore();
}

void PlayerPod::UpdateThrust()
{
	float newThrust = POD_MAXIMUM_THRUST;

	float turningMultiplier = 1.0f;
	bool isTurning = false;

	float brakingMultiplier = 1.0f;
	bool isBraking = false;

	// Slow down when the player is turning to the next checkpoint
	turningMultiplier = (1.0f - m_nextCheckpoint.m_angle / THRUST_ANGLE_BEFORE_TURNING);
	turningMultiplier = clamp(turningMultiplier, 0.0f, 1.0f);
	cerr << "Turning Multiplier : " << turningMultiplier << endl;
	m_isTurning = turningMultiplier <= 0.80f;

	// Slow down as the player get closer to the checkpoint.
	brakingMultiplier = m_nextCheckpoint.m_distance / THRUST_DISTANCE_BEFORE_BRAKING;
	brakingMultiplier = clamp(brakingMultiplier, THRUST_MINIMUM_BRACKING_MULTIPLIER, 1.0f);
	cerr << "Braking Multiplier : " << brakingMultiplier << endl;
	m_isBraking = brakingMultiplier <= 0.80f;

	m_thrust = newThrust * turningMultiplier * brakingMultiplier;
	cerr << "Thrust : " << m_thrust << endl;
}

void PlayerPod::UpdateSpeed()
{
	m_speed = m_position - m_lastPosition;
	m_lastPosition = m_position;
	cerr << "Speed : " << "x:" << m_speed.m_x << "y:" << m_speed.m_y << endl;
}

void PlayerPod::UpdateNextCheckpoint()
{
	cerr << "Angle to checkpoint : " << m_nextCheckpoint.m_angle << endl;
	cerr << "Distance to checkpoint : " << m_nextCheckpoint.m_distance << endl;
	m_nextCheckpoint.m_angle = abs(m_nextCheckpoint.m_angle);
}

void PlayerPod::UpdateTarget()
{
	// Prevent the player from going in the wrong direction at the start
	static bool isFirstFrame = true;
	if (isFirstFrame)
	{
		isFirstFrame = false;
		m_target = m_nextCheckpoint.m_position;
		return;
	}

	// Create an offset to the checkpoint position, taking into account of the player speed
	Vector2 offset = m_speed * -3.0f;
	m_target = m_nextCheckpoint.m_position + offset;
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
}

void PlayerPod::UpdateOpponentInteraction(const Pod& _opponent)
{
	// Calculate the dot product of the opponent and the player
	m_playerToTarget = m_target - m_position;
	m_playerToOpponent = _opponent.GetPosition() - m_position;
	m_dotOpponentPlayer = Vector2::Dot(m_playerToTarget.Normalized(), m_playerToOpponent.Normalized());
	cerr << "Dot between opponent and player : " << m_dotOpponentPlayer << endl;
}

void PlayerPod::SendOutput()
{
	if (CheckShouldBoost() == true)
	{
		cout << m_target << " " << BOOST_KEYWORD << " " << m_hoverText << endl;
	}
	else
	{
		cout << m_target << " " << m_thrust << " " << m_hoverText << endl;
	}
}

bool PlayerPod::CheckShouldBoost()
{
	if (false == m_canUseBoost) return false;

	bool hasGoodAngleToCheckpoint = m_nextCheckpoint.m_angle < BOOST_THRESHOLD_ANGLE;
	if (false == hasGoodAngleToCheckpoint) return false;

	bool hasGoodDistanceToCheckpoint = m_nextCheckpoint.m_distance > BOOST_THRESHOLD_DISTANCE_TO_CHECKPOINT;
	if (false == hasGoodDistanceToCheckpoint) return false;

	bool isInFrontOfOpponent = (m_dotOpponentPlayer > BOOST_THRESHOLD_DOT_OPPONENT);
	if (isInFrontOfOpponent)
	{
		bool hasGoodDistanceToOpponent = m_playerToOpponent.Magnitude() < BOOST_THRESHOLD_DISTANCE_TO_OPPONENT;
		if (false == hasGoodDistanceToOpponent)
		{
			return false;
		}
	}

	m_canUseBoost = false;
	return true;
}

#pragma endregion

int main()
{
	PlayerPod playerPod;
	Pod opponent;

	bool isRunning = true;
	while (isRunning)
	{
		playerPod.ReceiveInput();
		opponent.ReceiveInput();

		playerPod.UpdateThrust();
		playerPod.UpdateSpeed();
		playerPod.UpdateNextCheckpoint();
		playerPod.UpdateTarget();
		playerPod.UpdateOpponentInteraction(opponent);
		playerPod.UpdateHoverText();

		playerPod.SendOutput();
	}
}
