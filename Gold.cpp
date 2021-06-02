#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Ressources : https://www.youtube.com/watch?v=8kBQMQyLHME&t=845s
// Ressources : http://files.magusgeek.com/csb/csb.html
// Ressources : https://tcourreges.github.io/coders-strike-back

#define PI 3.141592
#define MINIMUM_INT -32767
#define MAXIMUM_INT 32767

#define FLOAT_COMPARE(_a, _p2) (fabs(_a - _p2) < 0.000001)
#define DEG_TO_RAD(angleDeg) (angleDeg * PI / 180.0)
#define RAD_TO_DEG(angleRad) (angleRad * 180.0 / PI)
#define LERP(a, b, t) (a * (1.0 - t)) + (b * t);

#define TARGET_DISTANCE 1000.0

#define NB_SIMULATED_POD 2
#define NB_TURN_SIMULATED 4
#define SOLUTIONS_COUNT 6
#define TIME_ALLOCATED_PER_TURN 60
#define ROTATION_CHANGE_BY_MUTATION 26.0
#define THRUST_CHANGE_BY_MUTATION 26.0
#define PROBABILITY_TO_USE_SHIELD 20
#define PROBABILITY_TO_USE_BOOST 20

#define EVALUATION_CHECKPOINT_FACTOR 40000

#pragma region Game Rules

#define BOOST_KEYWORD "Boost"

#define MAP_WIDTH 16000.0
#define MAP_HEIGHT 9000.0

#define POD_TOTAL_NB 4
#define POD_CONTROLLABLE_NB 2

#define POD_MAX_THRUST 100
#define POD_COLLIDER_SIZE 400.0
#define POD_MAXIMUM_ROTATION 18.0
#define POD_FRICTION 0.85f
#define POD_BOOST_ACCELERATION 650.0
#define POD_COLLISION_ACCELERATION 120.0
#define POD_MASS_MULTIPLIER_BY_SHIELD 10

#define CHECKPOINT_MAX_NB 8
#define CHECKPOINT_RADIUS 600.0

#pragma endregion 

#pragma region Random Class

class Random
{
public:

	inline static int Range(int _minimumValue, int _maximumValue);

private:

	inline static int GenerateNumber();
};

inline int Random::Range(int _minimumValue, int _maximumValue)
{
	return (GenerateNumber() % (_maximumValue - _minimumValue)) + _minimumValue;
}

inline int Random::GenerateNumber()
{
	static unsigned int m_seed;
	m_seed = (214013 * m_seed + 2531011);
	return (m_seed >> 16) & 0x7FFF;
}

#pragma endregion

#pragma region Vector2 Class

class Vector2
{
public:

	static const Vector2 Zero;
	static const Vector2 Up;
	static const Vector2 Right;

	Vector2() = default;
	Vector2(double _x, double _y = 0.0f) : m_x(_x), m_y(_y) {};
	Vector2(const Vector2& _other) : m_x(_other.m_x), m_y(_other.m_y) {};

	double m_x = 0;
	double m_y = 0.;

	static double Angle(const Vector2& _v1, const Vector2& _v2);
	static double Dot(const Vector2& _v1, const Vector2& _v2);
	static double Distance(const Vector2& _p1, const Vector2& _p2);
	static double SquareDistance(const Vector2& _p1, const Vector2& _p2);

	double Magnitude() const;
	inline double SquareMagnitude() const;
	Vector2 Normalized() const;

	inline bool operator== (const Vector2& _v) const { return (FLOAT_COMPARE(m_x, _v.m_x) && FLOAT_COMPARE(m_y, _v.m_y)); };
	inline bool operator!= (const Vector2& _v) const { return (!FLOAT_COMPARE(m_x, _v.m_x) || !FLOAT_COMPARE(m_y, _v.m_y)); };
	inline Vector2& operator+= (const Vector2& _v);
	inline Vector2& operator-= (const Vector2& _v);
	inline Vector2& operator*= (double _multiplier);
	inline Vector2& operator/= (double _divider);
	inline Vector2 operator-() const;
	inline Vector2 operator+ (const Vector2& _v) const;
	inline Vector2 operator- (const Vector2& _v) const;
	inline Vector2 operator* (double _multiplier) const;
	inline Vector2 operator/ (double _divider) const;

	friend ostream& operator<< (ostream& _o, const Vector2& _v);
};

const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::Up = Vector2(0.0f, -1.0f);
const Vector2 Vector2::Right = Vector2(1.0f, 0.0f);

double Vector2::Angle(const Vector2& _v1, const Vector2& _v2)
{
	double angle = 0;
	angle = acos(Dot(_v1, _v2));
	angle = RAD_TO_DEG(angle);
	return angle;
}

double Vector2::Dot(const Vector2& _v1, const Vector2& _v2)
{
	if (_v1 == Zero || _v2 == Zero) { return 0.0f; }
	return (_v1.m_x * _v2.m_x) + (_v1.m_y * _v2.m_y);
}

double Vector2::Distance(const Vector2& _p1, const Vector2& _p2)
{
	if (_p1 == _p2) return 0.0f;
	return sqrtf(SquareDistance(_p1, _p2));
}

double Vector2::SquareDistance(const Vector2& _p1, const Vector2& _p2)
{
	if (_p1 == _p2) return 0.0f;
	double distanceX = _p2.m_x - _p1.m_x;
	double distanceY = _p2.m_y - _p1.m_y;
	return (distanceX * distanceX) + (distanceY * distanceY);
}

double Vector2::Magnitude() const
{
	if (m_x == 0.0f && m_y == 0.0f) return 0.0f;
	double magnitude = SquareMagnitude();
	return sqrtf(magnitude);
}

double Vector2::SquareMagnitude() const
{
	return (m_x * m_x) + (m_y * m_y);
}

Vector2 Vector2::Normalized() const
{
	if (*this == Zero) return Zero;
	if (FLOAT_COMPARE(m_x, 0.0f)) return Vector2(0.0f, 1.0f);
	if (FLOAT_COMPARE(m_y, 0.0f)) return Vector2(1.0f, 0.0f);

	double magnitude = this->Magnitude();
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

Vector2& Vector2::operator*=(double _multiplier)
{
	m_x *= _multiplier;
	m_y *= _multiplier;
	return *this;
}

Vector2& Vector2::operator/=(double _divider)
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
Vector2 Vector2::operator*(double _multiplier) const
{
	Vector2 newVec = Vector2(*this);
	newVec *= _multiplier;
	return newVec;
}

Vector2 Vector2::operator/(double _divider) const
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

#pragma region Entity Class

class Entity
{
public:
	int m_index = 0;
	Vector2 m_position = Vector2::Zero;
	double m_radius = 0.0f;
	Vector2 m_speed = Vector2::Zero;
};

#pragma endregion

#pragma region Checkpoint Class

class Checkpoint : public Entity
{
public:

	void ReceiveInput(int _index);
};

void Checkpoint::ReceiveInput(int _index)
{
	m_index = _index;
	cin >> m_position.m_x >> m_position.m_y;
	cin.ignore();
}

#pragma endregion

#pragma region Pod Class

class Pod : public Entity
{
public:

	void ReceiveInput(int _index);

	inline Vector2 GetPosition() const { return m_position; }

	// Pod values
	bool m_isControllable = false;
	int m_angle = 0.0f; // Obtained from input

	// Race values
	int m_currentCheckpointIndex = 1;
	int m_checkpointPassedCount = 0;
};

void Pod::ReceiveInput(int _index)
{
	int newCheckpointIndex = 0;

	cin >> m_position.m_x >> m_position.m_y >> m_speed.m_x >> m_speed.m_y >> m_angle >> newCheckpointIndex;
	cin.ignore();

	m_index = _index;
	m_isControllable = (_index < POD_CONTROLLABLE_NB);

	if (newCheckpointIndex != m_currentCheckpointIndex)
	{
		cerr << "Pod " << m_index << " Checkpoint passed" << endl;
		m_checkpointPassedCount++;
		m_currentCheckpointIndex = newCheckpointIndex;
	}
}

#pragma endregion

#pragma region Solution Class and members stuctures

struct Move
{
	int m_rotation = 0; // From 0 to 100
	int m_thrust = 0; // From 0 to 100
	bool m_useBoost = false;
	bool m_useShield = false;
};

struct Turn
{
	array<Move, NB_SIMULATED_POD> m_moves;
};

class Solution
{
public:

	static Move GenerateMove(double _amplitude = 1.0);

	void ShiftTurn(double _amplitude = 1.0);

	int m_score = -1;
	array<Turn, NB_TURN_SIMULATED> m_turns;

private:
};

void Solution::ShiftTurn(double _amplitude)
{
	for (int iTurn = 1; iTurn < NB_TURN_SIMULATED; iTurn++)
	{
		for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
		{
			m_turns[iTurn - 1] = m_turns[iTurn];
		}
	}

	//create a new random turn
	for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
	{
		m_turns[NB_TURN_SIMULATED - 1].m_moves[iPod] = GenerateMove(_amplitude);
	}
}

Move Solution::GenerateMove(double _amplitude)
{
	Move move;

	///cerr << "Start to generate a move" << endl;

	// Rotation
	int minimumRotation = (int)(-POD_MAXIMUM_ROTATION * _amplitude);
	int maximumRotation = (int)(POD_MAXIMUM_ROTATION * _amplitude);
	move.m_rotation = Random::Range(minimumRotation, maximumRotation);

	// Shield
	/// move.m_useShield = (false == move.m_useShield) && (Random::Range(0, 100) < PROBABILITY_TO_USE_SHIELD);
	/// if (move.m_useShield) return move;

	// Boost
	/// move.m_useBoost = (false == move.m_useBoost) && (Random::Range(0, 100) < PROBABILITY_TO_USE_BOOST);
	/// if (move.m_useBoost) return move;

	// Thrust
	int random = Random::Range(0, 100);
	if (random < 60) { move.m_thrust = 100; }
	else if (random < 70) { move.m_thrust = 10; }
	else
	{
		int minimumThrust = move.m_thrust - (int)(THRUST_CHANGE_BY_MUTATION * _amplitude);
		int maximumThrust = move.m_thrust + (int)(THRUST_CHANGE_BY_MUTATION * _amplitude);
		if (minimumThrust < 0) minimumThrust = 0;
		if (maximumThrust > 100) maximumThrust = 100;
		move.m_thrust = Random::Range(minimumThrust, maximumThrust);
	}

	///cerr << "Move generated" << endl;

	return move;
}


bool CompareByScore(const Solution& a, const Solution& b)
{
	return a.m_score > b.m_score;
}

#pragma endregion

#pragma region Simulation Class

class Simulation
{
public:

	void InitializeCheckpoints();
	void ReceivePodsInputs(bool _isFirstTurn = false);
	void SimulateSolution(const Solution& _solution);
	void SendOutputFromSolution(const Solution& _solution);

	array<Pod, POD_TOTAL_NB> m_pods; // Pods currenly in game
	Checkpoint m_checkpoints[CHECKPOINT_MAX_NB];
	int m_numberOfLaps = 0;
	int m_checkpointCount_Lap = 0; // Checkpoints in one lap
	int m_checkpointCount_Race = 0; // Checkpoints in the race
	array<Pod, POD_TOTAL_NB> m_tempPods; // Temporary pods created for the current simulation

private:

	void SimulateBeforePhysics(const array<Move, POD_CONTROLLABLE_NB> _moves);
	void SimulatePhysics(const array<Move, POD_CONTROLLABLE_NB> _moves);
	void SimulateAfterPhysics(const array<Move, POD_CONTROLLABLE_NB> _moves);
};

void Simulation::InitializeCheckpoints()
{
	cin >> m_numberOfLaps;
	cin.ignore();
	cin >> m_checkpointCount_Lap;
	cin.ignore();
	for (size_t iCheckpoint = 0; iCheckpoint < m_checkpointCount_Lap; iCheckpoint++)
	{
		m_checkpoints[iCheckpoint].ReceiveInput(iCheckpoint);
	}
	m_checkpointCount_Race = m_checkpointCount_Lap * m_numberOfLaps;
	cerr << "Checkpoints Initialized" << endl;
}

void Simulation::ReceivePodsInputs(bool _isFirstTurn)
{
	for (size_t iPod = 0; iPod < POD_TOTAL_NB; iPod++)
	{
		Pod& pod = m_pods[iPod];

		pod.ReceiveInput(iPod);

		if (false == _isFirstTurn) continue;

		// Override the angle for the first turn to face the first checkpoint
		Vector2 direction = (m_checkpoints[pod.m_currentCheckpointIndex].m_position - pod.m_position).Normalized();
		float newAngle = acos(direction.m_x) * 180.0 / PI;
		if (direction.m_y < 0.0) newAngle = (360.0 - newAngle);
		pod.m_angle = newAngle;
	}

	///cerr << "Received inputs" << endl;
}

void Simulation::SimulateSolution(const Solution& _solution)
{
	m_tempPods = m_pods; // Copy the initial pods for the new solution
	for (int iTurn = 0; iTurn < NB_TURN_SIMULATED; iTurn++)
	{
		SimulateBeforePhysics(_solution.m_turns[iTurn].m_moves);
		SimulatePhysics(_solution.m_turns[iTurn].m_moves);
		SimulateAfterPhysics(_solution.m_turns[iTurn].m_moves);
	}
	///for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
	///{
	///	cerr << " pod is at " << m_pods[iPod].m_position << endl;
	///}
}

void Simulation::SendOutputFromSolution(const Solution& _solution)
{
	for (size_t iPod = 0; iPod < POD_CONTROLLABLE_NB; iPod++)
	{
		Pod& pod = m_pods[iPod];
		const Move& move = _solution.m_turns[0].m_moves[iPod];

		double angle = (pod.m_angle + move.m_rotation) % 360;
		double angleRad = DEG_TO_RAD(angle);
		Vector2 direction = Vector2(TARGET_DISTANCE * cos(angleRad), TARGET_DISTANCE * sin(angleRad));
		Vector2 target = pod.m_position + direction;

		string hoverHeadText = " ";
		if (move.m_useBoost) hoverHeadText += "SHIELD";
		if (move.m_useBoost) hoverHeadText += "BOOST";
		else hoverHeadText += "THRUST_" + to_string(move.m_thrust);
		hoverHeadText += " ANGLE_" + to_string(move.m_rotation);

		cout << target << " " << move.m_thrust << hoverHeadText << endl;
	}
}

void Simulation::SimulateBeforePhysics(const array<Move, NB_SIMULATED_POD> _moves)
{
	for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
	{
		Pod& pod = m_tempPods[iPod];
		const Move& move = _moves[iPod];

		pod.m_angle = (pod.m_angle + move.m_rotation) % 360;
		float angleRad = DEG_TO_RAD(pod.m_angle);
		Vector2 direction = Vector2(cos(angleRad), sin(angleRad));

		int thrust = move.m_thrust;
		pod.m_speed += direction * (double)thrust;
	}
}

void Simulation::SimulatePhysics(const array<Move, NB_SIMULATED_POD> _moves)
{
	double time = 0.0;
	double endTime = 1.0;
	while (time < endTime)
	{
		float deltaTime = endTime - time;

		for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
		{
			Pod& pod = m_tempPods[iPod];
			const Move& move = _moves[iPod];

			pod.m_position += pod.m_speed * deltaTime; // Move the pod

			// Check checkpoints collisions
			if (Vector2::SquareDistance(pod.m_position, m_checkpoints[pod.m_currentCheckpointIndex].m_position) < (CHECKPOINT_RADIUS * CHECKPOINT_RADIUS))
			{
				//cerr << "collision with checkpoint in sumulation" << endl;
				pod.m_currentCheckpointIndex = (pod.m_currentCheckpointIndex + 1) % m_checkpointCount_Lap;
				pod.m_checkpointPassedCount++;
			}
		}

		time += deltaTime;
	}
}

void Simulation::SimulateAfterPhysics(const array<Move, NB_SIMULATED_POD> _moves)
{
	for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
	{
		Pod& pod = m_tempPods[iPod];

		pod.m_speed *= POD_FRICTION;
		pod.m_position = Vector2(round(pod.m_position.m_x), round(pod.m_position.m_y));

		/*cerr << "pod position ref " << pod.m_position << endl;
		cerr << "pod position temp " << m_tempPods[iPod].m_position << endl;*/
	}
}

#pragma endregion

#pragma region Solver Class

class Solver
{
public:

	Solver(Simulation* _simulation);
	const Solution& Solve(double _amplitude = 1.0);

private:

	void GeneratePopulation();
	void Mutate(Solution* _solution, double _amplitude = 1.0);
	int EvaluateSolution(Solution* _solution, const Simulation& _simulation);

	Simulation* m_simulation = nullptr;
	vector<Solution> m_solutions;
	int m_minimumScore = -1;
};

Solver::Solver(Simulation* _simulation)
{
	m_simulation = _simulation;
	m_solutions.resize(SOLUTIONS_COUNT);
	GeneratePopulation();
}

void Solver::GeneratePopulation()
{
	cerr << "Start to generate population" << endl;
	for (int iSolution = 0; iSolution < SOLUTIONS_COUNT; iSolution++)
	{
		for (int iTurn = 0; iTurn < NB_TURN_SIMULATED; iTurn++)
		{
			for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
			{
				m_solutions[iSolution].m_turns[iTurn].m_moves[iPod] = Solution::GenerateMove(1.0);
			}
		}
	}
	cerr << "Population generated" << endl;
}

const Solution& Solver::Solve(double _amplitude)
{
	auto startTime = high_resolution_clock::now();
	int timepassed = 0;

	int nbSolutionCreated = 0;
	int nbSolutionFound = 0;

	int lastScore = -1;

	m_solutions.resize(SOLUTIONS_COUNT);
	for (size_t iSolution = 0; iSolution < SOLUTIONS_COUNT; iSolution++)
	{
		Solution& solution = m_solutions[iSolution];
		solution.ShiftTurn(_amplitude);
		m_simulation->SimulateSolution(solution);
		int currentScore = EvaluateSolution(&solution, *m_simulation);
		if (currentScore > lastScore) lastScore = currentScore;
	}

	while (timepassed < TIME_ALLOCATED_PER_TURN)
	{
		Solution solution = m_solutions[Random::Range(0, SOLUTIONS_COUNT)];
		Mutate(&solution, _amplitude);
		m_simulation->SimulateSolution(solution);
		int currentScore = EvaluateSolution(&solution, *m_simulation);
		nbSolutionCreated++;
		if (currentScore > lastScore)
		{
			lastScore = currentScore;
			m_solutions.push_back(solution);
			nbSolutionFound++;
		}
		timepassed = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	}

	cerr << nbSolutionFound << " good solutions have been found for " << nbSolutionCreated << " created." << endl;

	sort(m_solutions.begin(), m_solutions.end(), CompareByScore);

	cerr << "Wining solution have a score of " << m_solutions[0].m_score << endl;

	return m_solutions[0];
}

void Solver::Mutate(Solution* _solution, double _amplitude)
{
	for (size_t iTurn = 0; iTurn < NB_TURN_SIMULATED; iTurn++)
	{
		for (int iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
		{
			_solution->m_turns[iTurn].m_moves[iPod] = Solution::GenerateMove(_amplitude);
		}
	}
}

int Solver::EvaluateSolution(Solution* _solution, const Simulation& _simulation)
{
	int score = -1;

	// Score the most ahead pod
	for (size_t iPod = 0; iPod < NB_SIMULATED_POD; iPod++)
	{
		const Pod& pod = _simulation.m_tempPods[iPod];
		int distanceToCheckpoint = Vector2::SquareDistance(pod.m_position, _simulation.m_checkpoints[pod.m_currentCheckpointIndex].m_position) / 10000;
		score += EVALUATION_CHECKPOINT_FACTOR * (pod.m_checkpointPassedCount + 1) - distanceToCheckpoint;

		/*cerr << "pod position at end of simulation" << pod.m_position << endl;
		cerr << "pod thrust " << _solution->m_turns[0].m_moves[iPod].m_thrust << endl;
		cerr << "checkpoint position " << _simulation.m_checkpoints[pod.m_currentCheckpointIndex].m_position << endl;
		cerr << "distanceToCheckpoint " << distanceToCheckpoint << endl;*/
	}

	_solution->m_score = score;
	return score;
}

#pragma endregion

int main()
{
	bool isFirstTurn = true;

	Simulation simulation;
	Solver solver(&simulation);

	simulation.InitializeCheckpoints();

	while (1)
	{
		auto startTime = high_resolution_clock::now();

		simulation.ReceivePodsInputs(isFirstTurn);
		const Solution& solution = solver.Solve(1.0);

		cerr << "Time used for this frame = " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() << endl;

		simulation.SendOutputFromSolution(solution);
		isFirstTurn = false;
	}
}
