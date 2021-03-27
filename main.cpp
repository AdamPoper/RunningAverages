#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>

#define PARTS 30

struct Time
{
    float hours;
    float minutes;
    float seconds;
};
typedef Time Pace;

struct Run
{
    float distance;
    Pace pace;
    std::string date;
    Time time;
};

std::ostream& operator<<(std::ostream& os, const Run& r)
{
    os << r.date << " " << r.distance << " " << r.time.hours << ":" << r.time.minutes << ":" <<
    r.time.seconds << " " << r.pace.minutes << ":" << r.pace.seconds;
    return os;
}

template<typename T>
T convertFromString(const std::string& str)
{
    std::stringstream ss(str);
    T newValue = T();
    ss >> newValue;
    return newValue;
}

template<typename T, size_t size>
std::array<T, size> parseString(std::string& str, const std::string& delimiter)
{
    std::array<T, size> parts;
    uint32_t count = 0;
    std::string token;
    uint32_t pos = 0;
    while((pos = str.find(delimiter)) != std::string::npos && count < size-1)
    {
        token = str.substr(0, pos);
        str.erase(0, pos + delimiter.length());
        parts[count] = convertFromString<T>(token);
        count++;
    }
    parts[count] = convertFromString<T>(str);
    return parts;
}

Pace calcAverageTotalPace(const std::vector<Run>& runs)
{
    float totalSeconds = 0.0;
    float totalMiles = 0.0f;
    for(const Run& run : runs)
    {
        totalMiles += run.distance;
        float paceInSeconds = (run.pace.minutes * 60.0f) + run.pace.seconds;
        float totalRunInSeconds = paceInSeconds * run.distance;
        totalSeconds += totalRunInSeconds;
    }
    float averageInSeconds = totalSeconds / totalMiles;
    float averageMins = (float)((uint32_t)averageInSeconds / 60);
    float averageSecs = (float)fmod((double)averageInSeconds, 60.0);
    return {0.0f, averageMins, averageSecs};
}

float calcAverageRunDistance(const std::vector<Run>& runs)
{
    float totalDistance = 0.0f;
    for(const Run& run : runs)
        totalDistance += run.distance;
    float averageDistance = totalDistance / runs.size();
    return averageDistance;
}

float calcAverageDailyDistance(const std::vector<Run>& runs)
{
    float totalDistance = runs[0].distance;
    uint32_t totalDaysOfRunning = 1;
    for(int i = 1; i < runs.size(); i++)
    {
        totalDistance += runs[i].distance;
        if(runs[i].date != runs[i-1].date)
            totalDaysOfRunning++;
    }
    float avgDailyDistance = totalDistance / (float)totalDaysOfRunning;
    return avgDailyDistance;
}

Time calcAverageRunTime(const std::vector<Run>& runs)
{
    float totalSecondsRunning = 0.0f;
    for(const Run& run : runs)
    {
        float totalRunSecs = run.time.hours * 60.0f * 60.0f;
        totalRunSecs += (run.time.minutes * 60.0f);
        totalRunSecs += run.time.seconds;
        totalSecondsRunning += totalRunSecs;
    }
    float avgRunTotalSecs = totalSecondsRunning / runs.size();
    float avgRunHours = (float)((uint32_t)avgRunTotalSecs / 60 / 60);
    avgRunTotalSecs -= (avgRunHours * 60 * 60);
    float avgRunMins = (float)((uint32_t)avgRunTotalSecs / 60);
    float avgRunSecs = (float)fmod((double)avgRunTotalSecs, 60.0);
    return {avgRunHours, avgRunMins, avgRunSecs};
}

float calcAverageWeeklyMileage(const std::vector<Run>& runs)
{
    float totalDistance = runs[0].distance;    
    uint32_t daysRunning = 1;
    float numWeeks = 0.0f;
    for(int i = 1; i < runs.size(); i++)
    {
        totalDistance += runs[i].distance;
        if(runs[i].date != runs[i-1].date)
            daysRunning++;        
        if(daysRunning == 7)
        {
            numWeeks += 1.0f;
            daysRunning = 0.0f;
        }
    }
    if(daysRunning != 0)
        numWeeks += ((float)daysRunning / 7.0f);
    float averageWeeklyDistance = totalDistance / numWeeks;
    return averageWeeklyDistance;
}

int main()
{
    std::ifstream stream;
    stream.open("activities.csv");
    std::string line;    
    std::vector<Run> runs;
    runs.reserve(690);    
    int lineIndex = 0;
    if (!stream.fail())
    {
        while (getline(stream, line))
        {
            auto parsedLine = parseString<std::string, PARTS>(line, ",");        
            parsedLine[4].erase(std::remove(parsedLine[4].begin(), parsedLine[4].end(), '"'), parsedLine[4].end());
            parsedLine[12].erase(std::remove(parsedLine[12].begin(), parsedLine[12].end(), '"'), parsedLine[12].end());                
            auto parsedPace = parseString<float, 2>(parsedLine[12], ":");
            float distance = convertFromString<float>(parsedLine[4]);
            Pace pace = { 0.0f, parsedPace[0], parsedPace[1] };
            std::string date = parseString<std::string, 2>(parsedLine[1], " ")[0];
            parsedLine[6].erase(std::remove(parsedLine[6].begin(), parsedLine[6].end(), '"'), parsedLine[6].end());
            auto parsedTime = parseString<float, 3>(parsedLine[6], ":");
            Time time = {parsedTime[0], parsedTime[1], parsedTime[2]};       
            runs.push_back({ distance, pace, date, time });
        }            
    }
    else
        std::cout << "stream failed" << std::endl;

    stream.close();

/*
    for(const Run& run : runs)
        std::cout << run << std::endl;
*/
    Pace averageTotalPace = calcAverageTotalPace(runs);    

    std::cout << "Average Total Pace: " << averageTotalPace.minutes 
    << ":" << averageTotalPace.seconds << std::endl;
    
    float averageRunDistance = calcAverageRunDistance(runs);
    std::cout << "Average Run Distance: " << averageRunDistance << std::endl;

    float averageDailyDistance = calcAverageDailyDistance(runs);
    std::cout << "Average Daily Distance: " << averageDailyDistance << std::endl;

    Time avgRunTime = calcAverageRunTime(runs);
    std::cout << "Average Run Time: " << avgRunTime.hours << ":" << avgRunTime.minutes << ":" << avgRunTime.seconds << std::endl;

    float averageWeeklyMileage = calcAverageWeeklyMileage(runs);
    std::cout << "Average Weekly Mileage: " << averageWeeklyMileage << std::endl;
}


// Activity Type,Date,Favorite,Title,Distance,Calories,Time,Avg HR,Max HR,Aerobic TE,Avg Run Cadence,Max Run Cadence,Avg Pace,Best Pace,Elev Gain,Elev Loss,Avg Stride Length,Avg Vertical Ratio,Avg Vertical Oscillation,Training Stress Score®,Grit,Flow,Climb Time,Bottom Time,Min Temp,Surface Interval,Decompression,Best Lap Time,Number of Laps,Max Temp