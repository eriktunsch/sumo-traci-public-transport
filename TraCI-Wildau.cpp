#include <iostream>
#include <algorithm>

#include <libsumo/libtraci.h>
#include "tinyxml/tinyxml.h"

using namespace libtraci;
using namespace std;

int port = -1;
std::string sumocfg = "";
std::string sumoexe = "sumo-gui";

std::vector<std::tuple<std::string, int, int, int, int>> to_execute = {};

std::map<int, std::vector<std::string>> last_seen;
std::map<int, std::vector<std::string>> last_seen_class;
std::map<int, std::vector<std::string>> last_seen_temp;
std::map<int, std::vector<std::string>> vehicleClass;
std::map<int, std::vector<int>> phaseContinuation;
std::map<int, std::vector<std::string>> loops;
std::map<int, int> start_time;

class DetectSwicthDefine {
public:
    DetectSwicthDefine();
    int id;
    std::string tls;
    int prePhase;
    int delay;
    int finalPhase;
    int duration;
    int extraTime;
};

DetectSwicthDefine::DetectSwicthDefine() {}

std::vector<DetectSwicthDefine> instances = {};

void initConfig() {
    TiXmlDocument doc("traci.cfg.xml");
    doc.LoadFile();

    TiXmlElement *root = doc.RootElement();
    TiXmlElement *config = root->FirstChildElement("config");
    TiXmlElement *xinstances = root->FirstChildElement("instances");

    port = atoi(config->FirstChildElement("port")->Attribute("value"));
    sumocfg = config->FirstChildElement("sumocfg")->Attribute("value");
    sumoexe = config->FirstChildElement("sumoexe")->Attribute("value");

    for (TiXmlElement* e = xinstances->FirstChildElement("instance"); e != NULL; e = e->NextSiblingElement("instance")) {
        DetectSwicthDefine temp;
        temp.id = atoi(e->FirstChildElement("id")->Attribute("value"));
        temp.tls = e->FirstChildElement("tls")->Attribute("id");
        temp.prePhase = atoi(e->FirstChildElement("prePhase")->Attribute("index"));
        temp.delay = atoi(e->FirstChildElement("prePhase")->Attribute("delay"));
        temp.finalPhase = atoi(e->FirstChildElement("finalPhase")->Attribute("index"));
        temp.duration = atoi(e->FirstChildElement("finalPhase")->Attribute("duration"));
        temp.extraTime = atoi(e->FirstChildElement("finalPhase")->Attribute("extraTime"));

        last_seen.insert(std::pair<int, std::vector<std::string>>(0, {}));
        last_seen_temp.insert(std::pair<int, std::vector<std::string>>(0, {}));
        last_seen_class.insert(std::pair<int, std::vector<std::string>>(0, {}));

        TiXmlElement* detections = e->FirstChildElement("detection");
        TiXmlElement* continuations = e->FirstChildElement("phaseContinuations");
        TiXmlElement* xloops = e->FirstChildElement("loops");

        std::vector<std::string> temp2 = {};
        std::vector<int> temp3 = {};
        std::vector<std::string> temp4 = {};
       
        for (TiXmlElement* child = detections->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
            temp2.push_back(child->Attribute("class"));
        }

        for (TiXmlElement* child = continuations->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
            temp3.push_back(atoi(child->Attribute("index")));
        }

        for (TiXmlElement* child = xloops->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
            temp4.push_back(child->Attribute("id"));
        }

        vehicleClass.insert(std::pair<int, std::vector<std::string>>(temp.id, temp2));
        phaseContinuation.insert(std::pair<int, std::vector<int>>(temp.id, temp3));
        loops.insert(std::pair<int, std::vector<std::string>>(temp.id, temp4));

        start_time.insert(std::pair<int, int>(temp.id, 0));

        instances.push_back(temp);
    }
}

int main(int argc, char* argv[])
{
    std::cout << "Started!\n";
    initConfig();

    Simulation::start({ sumoexe, "-c", sumocfg, "--tripinfo-output", "trips.out.xml"}, port, 60, "default", true);

    while (true) {
        Simulation::step();
        for (DetectSwicthDefine const& instance : instances) {
            std::vector<std::string> ids = {};
            for (std::string const& loop : loops.find(instance.id)->second) {
                std::vector<std::string> temp_ids = InductionLoop::getLastStepVehicleIDs(loop);
                ids.insert(std::end(ids), std::begin(temp_ids), std::end(temp_ids));
            }

            last_seen_temp.find(instance.id)->second.clear();
            last_seen_class.find(instance.id)->second.clear();

            for (std::string const& value : ids) {
                if (!std::count(last_seen.find(instance.id)->second.begin(), last_seen.find(instance.id)->second.end(), value)) {
                    std::cout << value + " (" + Vehicle::getVehicleClass(value) + ")\n";
                    last_seen_class.find(instance.id)->second.push_back(Vehicle::getVehicleClass(value));
                }
                last_seen_temp.find(instance.id)->second.push_back(value);
            }

            last_seen.find(instance.id)->second = last_seen_temp.find(instance.id)->second;

            for (std::string const& value : vehicleClass.find(instance.id)->second) {
                if (std::find(last_seen_class.find(instance.id)->second.begin(), last_seen_class.find(instance.id)->second.end(), value) != last_seen_class.find(instance.id)->second.end()){
                    std::cout << value + " detected\n";
                    if (std::count(phaseContinuation.find(instance.id)->second.begin(), phaseContinuation.find(instance.id)->second.end(), TrafficLight::getPhase(instance.tls))) {
                        if (Simulation::getCurrentTime() - (start_time.find(instance.id)->second + (instance.duration * 1000)) < instance.extraTime * 1000) {
                            std::cout << "Duration + " + std::to_string(instance.extraTime);
                            TrafficLight::setPhase(instance.tls, TrafficLight::getPhase(instance.tls));
                            TrafficLight::setPhaseDuration(instance.tls, instance.extraTime);
                            start_time.find(instance.id)->second = Simulation::getCurrentTime() + (instance.delay * 1000) + (instance.duration * 1000);
                        } else if ((start_time.find(instance.id)->second + (instance.duration * 1000)) + (instance.extraTime*1000) < Simulation::getCurrentTime()) {
                            std::cout << "Duration + " + std::to_string(instance.extraTime);
                            TrafficLight::setPhase(instance.tls, TrafficLight::getPhase(instance.tls));
                            TrafficLight::setPhaseDuration(instance.tls, instance.extraTime);
                            start_time.find(instance.id)->second = Simulation::getCurrentTime() + (instance.delay * 1000) + (instance.duration * 1000);
                        }
                    } else {
                        TrafficLight::setPhase(instance.tls, instance.prePhase);
                        TrafficLight::setPhaseDuration(instance.tls, instance.delay);
                        std::cout << "Phase set " + std::to_string(instance.prePhase);
                        std::cout << "\n";
                        start_time.find(instance.id)->second = Simulation::getCurrentTime() + (instance.delay * 1000) + (instance.duration * 1000);
                        to_execute.push_back(std::tuple<std::string, int, int, int, int>(instance.tls, instance.finalPhase, instance.duration, Simulation::getCurrentTime(), instance.delay));
                    }
                }
            }
        }
        for (std::tuple<std::string, int, int, int, int> const& value : to_execute) {
            int ctime = std::get<3>(value) + (std::get<4>(value) * 1000);
            if (ctime == Simulation::getCurrentTime()) {
                TrafficLight::setPhase(std::get<0>(value), std::get<1>(value));
                TrafficLight::setPhaseDuration(std::get<0>(value), std::get<2>(value));
                std::cout << "Phase set " + std::to_string(std::get<1>(value));
                std::cout << "\n";
            }
        }
    }
    Simulation::close();
}
