#ifndef REPORTING_H
#define REPORTING_H

#include "database.h"

class Reporting : public Database {
public:
    struct ReportParams {
        std::string start_date;
        std::string end_date;
        std::optional<std::string> jockey_id;
        std::optional<std::string> owner_id;
    };

    Reporting(sqlite3* db);
    
    void generate_jockey_report(const ReportParams& params);
    void generate_owner_report(const ReportParams& params);
    void generate_financial_report(const ReportParams& params);
    void export_to_csv(const std::string& report_data, 
                      const std::string& filename);
};

#endif