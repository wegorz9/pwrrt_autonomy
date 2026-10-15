

#include <format>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>

class Point {
public:
    double timestamp;
    double x;
    double y;
    Point(double timestamp, double x, double y) {
        this-> timestamp = timestamp;
        this-> x = x;
        this-> y = y;
    }
};

// parsing a given csv according to the format provided in the task sheet
std::vector<Point> parseCsv (const std::string& filename) {
    std::vector<Point> data;
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) throw std::runtime_error("Could not open file: " + filename);

    std::string line;
    std::getline(inputFile, line);

    while (std::getline(inputFile, line)) {
        double timestamp;
        double x;
        double y;
        {
            std::string timestr = line.substr(0, line.find(','));
            line = line.substr(line.find(',')+1);

            std::stringstream ss (timestr);
            ss >> timestamp;
        }

        {
            std::string xstr = line.substr(0, line.find(','));
            line = line.substr(line.find(',')+1);

            std::stringstream ss (xstr);
            ss >> x;
        }

        {
            std::string ystr = line.substr(0, line.find(','));

            std::stringstream ss (ystr);
            ss >> y;
        }

        data.push_back(Point(timestamp,x,y));
    }
    inputFile.close();
    data.pop_back();
    return data;
}

// simple linear interpolation between data points
Point interpolatePosition(double timestamp, const std::vector<Point> &data) {
    // we do not extrapolate because that was not in the problem description
    if (timestamp < data.front().timestamp) return data.front();

    for (int i = 0; i < data.size()-1; ++i) {
        double t0 = data[i].timestamp;
        double x0 = data[i].x;
        double y0 = data[i].y;

        double t1 = data[i+1].timestamp;
        double x1 = data[i+1].x;
        double y1 = data[i+1].y;

        if (timestamp >= t0 && timestamp <= t1) {
            double alpha = (timestamp - t0) / (t1 - t0);
            double x = x0 + alpha * (x1-x0);
            double y = y0 + alpha * (y1-y0);
            return Point(timestamp, x, y);
        }
    }
    // if its not t = 0, and not somewhere in the middle, return the last point
    return data.back();
}

int main(int argc, char** argv) {
    if (argc < 3) throw std::runtime_error(std::format("Incorrect number of arguments, expected 2 got {}", argc-1));

    std::vector<Point> sensor1Data = parseCsv(argv[1]);
    std::vector<Point> sensor2Data = parseCsv(argv[2]);

    double requestedTimestamp;
    std::cout << "Enter t: ";
    if (!(std::cin >> requestedTimestamp)) throw std::runtime_error("T must be a number");
    if (requestedTimestamp < 0 || requestedTimestamp > 33) throw std::runtime_error(std::format("Expected value in range [0, 33], got {}", requestedTimestamp));

    // the data we're supposed to use was not specified in the problem description so I took the liberty to only use
    // sensor1 data because it's more frequent, so the linear interpolation will introduce less error
    Point interpolatedPosition = interpolatePosition(requestedTimestamp, sensor1Data);
    std::cout << std::format("Point(t: {}, x: {}, y: {})",
        interpolatedPosition.timestamp,
        interpolatedPosition.x,
        interpolatedPosition.y
        ) << std::endl;



    return 0;
}