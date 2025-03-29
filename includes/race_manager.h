#ifndef RACE_MANAGER_H
#define RACE_MANAGER_H

#include "database.h"

class RaceManager : public Database {
private:
    void validate_participants(const std::string& horse_id, 
                             const std::string& jockey_id);
                             
public:
    RaceManager(sqlite3* db);
    
    void schedule_race(const std::string& date, 
                      const std::vector<std::pair<std::string, std::string>>& participants);
    
    void record_results(const std::string& race_id, 
                       const std::vector<std::string>& rankings);
    
    void distribute_prizes(const std::string& race_id, 
                          double prize_fund);
    
    void generate_race_report(const std::string& start_date, 
                            const std::string& end_date);
};

#endif