#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
enum lifttype
{
    passenger,
    stretcher
};
struct costresult
{
    int points;
    string reason;
};
struct Lift
{
    int currentfloor;
    int direction; //-1 down, 0 stationery, 1 up
    bool ismoving;
    bool hasstop;
    lifttype type;
};
struct liftscore
{
    int distance;
    int direction;
    int interruption;
    int abruptstop;
    int total;
    string directionreason;
    string interruptionreason;
    string abruptstopreason;
};
struct Dispatchresult
{
    int winningindex;
    vector<liftscore> scores;
};
class LiftDispatcher
{
private:
    vector<Lift> lifts;
    bool isheadingtowardscaller(const Lift &l, int callerfloor);
    int distancecost(const Lift &l, int callerfloor);
    costresult directioncost(const Lift &l, int callerfloor, int callerdirection);
    costresult interruptioncost(const Lift &l);
    costresult abruptstop(const Lift &l, int callerfloor);

public:
    LiftDispatcher(vector<Lift> initiallifts);
    Dispatchresult dispatch(int callerfloor, int callerdirection);
};
bool LiftDispatcher::isheadingtowardscaller(const Lift &l, int callerfloor)
{
    return (l.direction == 1 && l.currentfloor <= callerfloor) || (l.direction == -1 && l.currentfloor >= callerfloor);
}
LiftDispatcher::LiftDispatcher(vector<Lift> initiallifts) : lifts(initiallifts) {}
int LiftDispatcher::distancecost(const Lift &l, int callerfloor)
{
    return abs(l.currentfloor - callerfloor);
}
costresult LiftDispatcher::directioncost(const Lift &l, int callerfloor, int callerdirection)
{
    if (!l.ismoving)
        return {0, "Idle lift"}; // static lift

    if (l.direction == callerdirection && isheadingtowardscaller(l, callerfloor))
    {
        return {-2, "Already moving towards caller and same direction - Bonus"};
    }
    return {4, "Moving away from the caller or opposite direction - Penalty"};
}
costresult LiftDispatcher::interruptioncost(const Lift &l)
{
    if (l.ismoving && l.hasstop)
    {
        return {3, "Has a commited stop - Penalty"};
    }
    return {0, "Has no stop - No Penalty"};
}
costresult LiftDispatcher::abruptstop(const Lift &l, int callerfloor)
{
    int gap = abs(l.currentfloor - callerfloor);
    if (l.ismoving && gap < 2 && isheadingtowardscaller(l, callerfloor))
    {
        return {8, "Too close to stop smoothly - Penalty"};
    }
    return {0, "No abrupt stop scenario - No Penalty"};
}

Dispatchresult LiftDispatcher::dispatch(int callerfloor, int callerdirection)
{
    vector<liftscore> scores(lifts.size());
    for (int i = 0; i < lifts.size(); i++)
    {
        const Lift &l = lifts[i];
        liftscore s;
        s.distance = distancecost(l, callerfloor);
        costresult dir = directioncost(l, callerfloor, callerdirection);
        s.direction = dir.points;
        s.directionreason = dir.reason;
        costresult interrupt = interruptioncost(l);
        s.interruption = interrupt.points;
        s.interruptionreason = interrupt.reason;
        costresult abrupt = abruptstop(l, callerfloor);
        s.abruptstop = abrupt.points;
        s.abruptstopreason = abrupt.reason;
        s.total = s.abruptstop + s.direction + s.distance + s.interruption;
        scores[i] = s;
    }
    int winningindex = 0;
    for (int i = 1; i < lifts.size(); i++)
    {
        if (scores[i].total < scores[winningindex].total)
        {
            winningindex = i;
        }
        else if (scores[i].total == scores[winningindex].total && lifts[i].type == passenger && lifts[winningindex].type != passenger)
        {
            winningindex = i; // passenger prefered over service lift
        }
    }
    return {winningindex, scores};
}
void printres(const vector<Lift> &lifts, const Dispatchresult &result)
{
    cout << "\n\n=======Dispatch Results=======\n\n";
    for (int i = 0; i < lifts.size(); i++)
    {
        const liftscore &s = result.scores[i];
        cout << "\n Lift " << i + 1 << " (" << (lifts[i].type == passenger ? "Passenger" : "Stretcher") << ") :\n";
        cout << "Distance Cost:    " << s.distance << "\n\n";
        cout << "Direction Cost:     " << s.direction << "\n Direction Reason :   " << s.directionreason << "\n\n";
        cout << "Interruption Cost:      " << s.interruption << "\n Interruption Reason :   " << s.interruptionreason << "\n\n";
        cout << "Abrupt Stop Cost:      " << s.abruptstop << "\n Abrupt Stop Reason :   " << s.abruptstopreason << "\n\n";
        cout << "Total:      " << s.total << "\n";
    }
    cout << "\n >>>>> Lift " << result.winningindex + 1 << " is dispatched\n";
}
int main()
{
    int n;
    cout << "Enter the number of lifts : ";
    cin >> n;
    vector<Lift> lifts;
    for (int i = 0; i < n; i++)
    {
        Lift l;
        cout << "\n------- Lift " << i + 1 << "-------\n";
        int choice;
        cout << "Enter lift type ( 1= Stretcher/Serice, 2= Passenger):";
        cin >> choice;
        switch (choice)
        {
        case 1:
            l.type = stretcher;
            break;
        case 2:
            l.type = passenger;
            break;
        default:
            cout << "Invalid choice, defaulting to Passenger";
            l.type = passenger;
        }
        cout << "Enter currentfloor : ";
        cin >> l.currentfloor;
        int moving;
        cout << "Is the lift currently moving? (Press 1 if moving or 0 if not) : ";
        cin >> moving;
        l.ismoving = (moving == 1);
        if (l.ismoving)
        {
            cout << "Enter Directiion of the lift (1 = up, -1 = down) : ";
            cin >> l.direction;
            int stops;
            cout << "Does it already have a commited Stop?(1 = Yes, 0 = No)";
            cin >> stops;
            l.hasstop = (stops == 1);
        }
        else
        {
            l.direction = 0;
            l.hasstop = false;
        }
        lifts.push_back(l);
    }
    LiftDispatcher dispatcher(lifts);
    int callerfloor, callerdirection;
    cout << "Enter caller floor :";
    cin >> callerfloor;
    cout << "Enter caller direction :";
    cin >> callerdirection;
    Dispatchresult result = dispatcher.dispatch(callerfloor, callerdirection);
    printres(lifts, result);
    return 0;
}