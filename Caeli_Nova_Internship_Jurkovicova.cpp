//Route finder 09/04/2021
//Michaela Jurkovicova
//Caeli Nove Software Internship

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>

using namespace std;

struct Airport {
    string name;
    double lat;
    double lon;
};

double calculateCentralAngle(Airport airport1, Airport airport2) {
    double dlat;
    double dlon;
    double angle;
    dlat =  (airport2.lat - airport1.lat);
    dlon = (airport1.lon - airport2.lon);
    //calculating angle from dot product
    angle = acos(sin(airport1.lat)*sin(airport2.lat) + (cos(airport1.lat)*cos(airport2.lat)*cos(dlon)));
    return angle;
}

vector<tuple<double, double>> calculatePathCoordinates(Airport airport1, Airport airport2) {
    double const1;
    double const2;
    double x,y,z;
    double intermediateLat;
    double intermediateLong;
    double fract;
    vector<tuple<double, double>> wayPoints;
    double angle = calculateCentralAngle(airport1, airport2);
    double stepSize = 0.0015;
    int numOfSteps = angle / stepSize;

    for(double i = 0; i < (numOfSteps + 1); i++) {
        fract = i * stepSize / angle;
        const1 = sin((1 - fract) * angle) / sin(angle);
        const2 = sin(fract * angle) / sin(angle);
        x = const1 * cos(airport1.lat) * cos(airport1.lon) + const2 * cos(airport2.lat) * cos(airport2.lon);
        y = const1 * cos(airport1.lat) * sin(airport1.lon) + const2 * cos(airport2.lat) * sin(airport2.lon);
        z = const1 * sin(airport1.lat) + const2 * sin(airport2.lat);
        intermediateLat = atan2(z, sqrt(pow(x, 2) + pow(y, 2))) * 180 / M_PI;
        intermediateLong = atan2(y, x) * 180 / M_PI;
        wayPoints.push_back({intermediateLat, intermediateLong});
    }
    wayPoints.push_back({airport2.lat * 180 / M_PI, airport2.lon * 180 / M_PI});
    return wayPoints;
}

void printCoordinatesJSON(vector<tuple<double, double>> wayPoints) {
   cout << "var points = [";
    for(int i = 0; i < wayPoints.size() - 2; i++) {
       cout << "[" << get<0>(wayPoints[i]) << "," << get<1>(wayPoints[i]) << "]" << ",";
    }
    cout << "[" << get<0>(wayPoints[wayPoints.size() - 1]) << "," << get<1>(wayPoints[wayPoints.size() - 1]) << "]]" << endl;
}

vector<Airport> loadDatabase() {
    string temp;
    string header;
    string line;
    string data;
    vector<Airport> loadedDatabase;
    vector<string> row;
    
    fstream database;
    database.open("airports.csv", ios::in);
    getline(database, header);
    while (getline(database, line)) {
        row.clear();
        stringstream ss(line);
        while (getline(ss, data, ',' )) {
            row.push_back(data);
        }

        loadedDatabase.push_back({row[0], stod(row[1]) * M_PI / 180.0, stod(row[2])* M_PI / 180.0});
    }
    return loadedDatabase;
}

int main(int argc, char **argv) {
    vector<Airport> loadedDatabase;
    vector<tuple<double, double>> wayPoints;
    vector<string> userAirports = {argv[1], argv[2]};
    Airport airport1; 
    Airport airport2;
    loadedDatabase = loadDatabase();
        
    for(Airport airport : loadedDatabase) { 
        if (userAirports[0] == airport.name) 
            airport1 = airport;
        if (userAirports[1] == airport.name) 
            airport2 = airport;    
    }
        
     if ((airport1.name.empty()) || (airport2.name.empty())) {
         cout << "Airport(s) not found" << endl;
         return 1;
     }

     else {
        wayPoints = calculatePathCoordinates(airport1, airport2);
        printCoordinatesJSON(wayPoints);
     }

    return 0;
}



//references: https://mathworld.wolfram.com/GreatCircle.html
//            http://www.edwilliams.org/avform147.htm#Intermediate

