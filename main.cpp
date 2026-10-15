#include <cmath>
#include <format>

#include <fstream>
#include <sstream>
#include <vector>


class  Cone {
public:
    int id;
    double x;
    double y;
    Cone(const int id, const double x, const double y) {
        this->id = id;
        this->x = x;
        this->y = y;
    }
    std::string _str_() {
        return std::format("Cone(id: {}, x: {}, y: {})", this->id, this->x, this->y);
    }
    double distance(const Cone cone2) {
        return std::sqrt((cone2.x - this->x) * (cone2.x - this->x) + (cone2.y - this->y) * (cone2.y - this->y));
    }
    bool operator==(const Cone &cone) const {
        if (this->id != cone.id) return false;
        if (this->x != cone.x) return false;
        if (this->y != cone.y) return false;
        return true;
    }
    static double cross(Cone a, Cone b, Cone c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }
};

class ReplacementCone {
public:
    Cone cone = Cone(0, 0, 0);
    std::vector<int> idsOfRmvdCones;
    ReplacementCone(Cone cone, std::vector<int> idsOfRmvdCones) {
        this->cone = cone;
        this->idsOfRmvdCones = idsOfRmvdCones;
    }
};

// convex hull algorythm by monotone chain
// https://www.geeksforgeeks.org/dsa/convex-hull-monotone-chain-algorithm/
std::vector<Cone> findConvexHull(std::vector<Cone> cones) {
    if (cones.size() < 3) return {};

    // Sort by x, then by y using bubble sort.
    for (int i = 0; i < cones.size(); ++i) {
        for (int j = 0; j < cones.size() - 1 - i; ++j) {
            if (cones[j].x > cones[j + 1].x ||
                (cones[j].x == cones[j + 1].x &&
                 cones[j].y > cones[j + 1].y)) {
                std::swap(cones[j], cones[j + 1]);
                 }
        }
    }

    std::vector<Cone> hull;

    // Build the lower hull.
    for (const auto& cone : cones) {
        while (hull.size() >= 2 &&
               Cone::cross(hull[hull.size() - 2],
                           hull.back(), cone) <= 0) {
            hull.pop_back();
                           }

        hull.push_back(cone);
    }

    // Build the upper hull.
    const size_t lowerSize = hull.size();

    for (int i = static_cast<int>(cones.size()) - 2; i >= 0; --i) {
        while (hull.size() > lowerSize &&
               Cone::cross(hull[hull.size() - 2], hull.back(), cones[i]) <= 0) {
            hull.pop_back();
        }

        hull.push_back(cones[i]);
    }

    // Remove the repeated starting point.
    hull.pop_back();

    // No polygonal area if all points are collinear.
    if (hull.size() < 3) return {};

    return hull;

}

// bubble sort (by id) since our lists are short
static void sortConeVectors(std::vector<Cone> &cones) {
    for (int i = 0; i < cones.size(); ++i) {
        for (int j = 0; j < cones.size()-1-i; ++j) {
            if (cones[j].id > cones[j+1].id) {
                std::swap(cones[j], cones[j+1]);
            }
        }
    }
}

// checks if 2 vectors are the same (same entries in the same places)
static bool compareConeVectors(std::vector<Cone> cones1, std::vector<Cone> cones2) {
    if (cones1.size() != cones2.size()) return false;
    for (int i = 0; i < cones1.size(); i++) {
        if (cones1[i] != cones2[i]) return false;
    }
    return true;
}

// reads the list of cones into the provided vector and returns the minimum allowed distance between readings
static double readConesFromFile(char* filename, std::vector<Cone> &cones) {
    int NUMBER_OF_CONES;
    double MIN_DISTANCE;
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) throw std::runtime_error(std::format("Could not open file: {}", filename));
    std::string line;

    {
        std::getline(inputFile, line);

        //convert string to int
        std::stringstream ss( line );
        ss >> NUMBER_OF_CONES;
    }

    {
        std::getline(inputFile, line);

        //convert string to double
        std::stringstream ss( line );
        ss >> MIN_DISTANCE;
    }

    for (int i = 0; i < NUMBER_OF_CONES; i++) {
        std::getline(inputFile, line);

        // read the id from the line
        int id;
        {
            std::string idstr = line.substr(0, line.find(' '));
            line = line.substr(line.find(' ')+1);

            std::stringstream ss (idstr);
            ss >> id;
        }

        double x;
        {
            // read the x coord from the line
            std::string xstr = line.substr(0, line.find(' '));
            line = line.substr(line.find(' ')+1);
            std::stringstream ss (xstr);
            ss >> x;

        }
        double y;
        {
            // as with the previous 2 but for y
            std::string ystr = line.substr(0, line.find(' '));
            std::stringstream ss (ystr);
            ss >> y;
        }
        Cone cone(id, x, y);

        cones.push_back(cone);

    }
    inputFile.close();
    return  MIN_DISTANCE;
}


int main(int argc, char** argv) {
    if (argc != 2) throw std::runtime_error(std::format("Expected 1 argument, got {}", argc-1));

    std::vector<Cone> cones;
    double MIN_DISTANCE = readConesFromFile(argv[1], cones);
    std::vector<std::vector<Cone>> conesToDeDuplicate;

    // all possible options for culling later
    // this is not an optimal solution and its O(n^2), but it should work fine for this task
    // should performance prove to be unsatisfactory, any number of algorithms reducing the number of comparisons may be used (like splitting the space into chunks)
    for (auto& cone1 : cones) {
        std::vector<Cone> nearbyCones;


        for (auto& cone2 : cones) {
            if (cone1.id != cone2.id && cone1.distance(cone2) < MIN_DISTANCE)
                nearbyCones.push_back(cone2);

        }

        if (!nearbyCones.empty()) {
            nearbyCones.push_back(cone1);
            sortConeVectors(nearbyCones);
            conesToDeDuplicate.push_back(nearbyCones);
        }
    }

    // culling

    std::vector<std::vector<Cone>> conesToCull;

    for (auto coneVector : conesToDeDuplicate) {
        if (coneVector.empty()) continue;

        sortConeVectors(coneVector);

        bool doesExist = false;

        for (const auto& culledConeVec : conesToCull) {
            if (compareConeVectors(coneVector, culledConeVec)) {
                doesExist = true;
                break;
            }
        }

        if (!doesExist) {
            conesToCull.push_back(coneVector);
        }
    }

    // creating the new cones from the extracted entries
    int newId = cones.back().id + 1;
    std::vector<ReplacementCone> newCones;
    for (auto newCone: conesToCull) {
        double sumx = 0;
        double sumy = 0;
        std::vector<int> replacedIds;
        for (auto cone: newCone) {
            sumx += cone.x;
            sumy += cone.y;
            replacedIds.push_back(cone.id);
        }
        newCones.push_back(ReplacementCone(Cone(newId++, sumx/newCone.size(), sumy/newCone.size()), replacedIds));
    }

    // START OF TASK 1.2

    //actually creating an updated list of cones after deduplication
    std::vector<int> listOfIdsToRemove;
    for (auto newCone: newCones) {
        for (auto id : newCone.idsOfRmvdCones) {
            listOfIdsToRemove.push_back(id);
        }
    }
    std::vector<Cone> purgedConesList;
    for (auto cone : cones) {
        bool toDelete = false;
        for (auto id : listOfIdsToRemove) {
            if (cone.id == id) {
                toDelete = true;
                break;
            }
        }
        if (toDelete) continue;
        purgedConesList.push_back(cone);
    }
    for (auto newCone : newCones) {
        purgedConesList.push_back(newCone.cone);
    }
    cones.swap(purgedConesList);
    sortConeVectors(cones);

    // running the monotone chain algorithm
    std::vector<Cone> hull = findConvexHull(cones);

    // hull perimeter
    double enclosureLength = 0;

    if (!hull.empty()) {
        for (size_t i = 0; i < hull.size(); ++i) {
            enclosureLength += hull[i].distance(
                hull[(i + 1) % hull.size()]
            );
        }
    }

    // Console output
    for (auto newCone: newCones) {
        printf("CONE %d %f %f [", newCone.cone.id, newCone.cone.x, newCone.cone.y);
        for (auto id : newCone.idsOfRmvdCones) {
            printf("%d ", id);
        }
        printf("]\n");
    }

    printf("ENCLOSURE %.3f", enclosureLength);
    printf(" [");
    for (auto cone: hull) {
        printf("%d ", cone.id);
    }
    printf("]\n");
    return 0;
}
