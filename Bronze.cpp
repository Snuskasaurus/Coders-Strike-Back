#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

#define MAP_RADIUS_CHECKPOINT 600.0f
#define MAP_WIDTH 16000.0f
#define MAP_HEIGHT 9000.0f

#define BOOST_THRESHOLD_DISTANCE 8500.0f
#define BOOST_THRESHOLD_ANGLE 1.0f
#define BOOST_KEYWORD "BOOST"

#define FLOAT_COMPARE(_a, _b) (fabs(_a - _b) < 0.000001f)

#pragma region Vector2

class Vector2
{
public:
	static const Vector2 Zero;
	static const Vector2 Up;
	static const Vector2 Right;

	Vector2() = default;
	Vector2(int _x, int _y = 0) : m_x(_x), m_y(_y) {};
	Vector2(const Vector2& _other) : m_x(_other.m_x), m_y(_other.m_y) {};

	float m_x = 0;
	float m_y = 0.;

	static float Dot(const Vector2& _v1, const Vector2& _v2);
	static float Angle(const Vector2& _from, const Vector2& _to);
	static float Distance(const Vector2& _a, const Vector2& _b);
	static float SquareDistance(const Vector2& _a, const Vector2& _b);
	static Vector2 Normalize(const Vector2& _v);

	float Magnitude() const;
	inline float SquareMagnitude() const;

	inline bool operator== (const Vector2& _v) const { return (FLOAT_COMPARE(m_x, _v.m_x) && FLOAT_COMPARE(m_y, _v.m_y)); };
	inline bool operator!= (const Vector2& _v) const { return (!FLOAT_COMPARE(m_x, _v.m_x) || !FLOAT_COMPARE(m_y, _v.m_y)); };
	inline Vector2& operator+= (const Vector2& _v);
	inline Vector2& operator-= (const Vector2& _v);
	inline Vector2& operator*= (float _multiplier);
	inline Vector2& operator/= (float _divider);
	inline Vector2 operator-() const;
	inline Vector2 operator+ (const Vector2& _v) const;
	inline Vector2 operator- (const Vector2& _v) const;

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

float Vector2::Angle(const Vector2& _from, const Vector2& _to)
{
	Vector2 vec = _to - _from;
	return atan2f(vec.m_x, vec.m_y);
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

Vector2 Vector2::Normalize(const Vector2& _v)
{
	if (_v == Zero) return 0.0f;
	if (_v.m_x == 0.0f) return Vector2(0.0f, 1.0f);
	if (_v.m_y == 0.0f) return Vector2(1.0f, 0.0f);

	float magnitude = _v.Magnitude();
	return Vector2(_v.m_x / magnitude, _v.m_y / magnitude);
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

ostream& operator<<(ostream& _output, const Vector2& _v)
{
	_output << (int)_v.m_x << " " << (int)_v.m_y;
	return _output;
}

#pragma endregion

#pragma region Checkpoint

struct Checkpoint
{
public:
	Vector2 m_position = Vector2::Zero; // position of the next check point
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
	Vector2 m_position = Vector2::Zero;
	int m_thrust = 100;
	bool m_canUseBoost = true;
};

void Pod::ManageInput()
{
	cin >> m_position.m_x >> m_position.m_y;
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
	cin >> m_position.m_x >> m_position.m_y >> 
		m_nextCheckpoint.m_position.m_x >> m_nextCheckpoint.m_position.m_y >> 
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
	if (CheckShouldBoost() == true)
	{
		cout << m_nextCheckpoint.m_position << " " << BOOST_KEYWORD << endl;
	}
	else
	{
		m_thrust = ComputeThrust();
		cout << m_nextCheckpoint.m_position << " " << m_thrust << endl;
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

	bool hasGoodDistance = m_nextCheckpoint.m_distance > BOOST_THRESHOLD_DISTANCE;
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