#ifndef LEVENSHTEIN_DISTANCE_HPP
#define LEVENSHTEIN_DISTANCE_HPP

#include "metric.hpp"
#include <string>
#include <vector>
#include <algorithm>

class LevenshteinDistance : public Metric<std::string> {
public:
    double distance(const std::string& a, const std::string& b) const override {
        size_t m = a.size();
        size_t n = b.size();
        
        // Tabla de programación dinámica
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

        for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
        for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

        for (size_t i = 1; i <= m; ++i) {
            for (size_t j = 1; j <= n; ++j) {
                if (a[i - 1] == b[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] = 1 + std::min({dp[i - 1][j],     // Eliminación
                                             dp[i][j - 1],     // Inserción
                                             dp[i - 1][j - 1]}); // Sustitución
                }
            }
        }
        return static_cast<double>(dp[m][n]);
    }
};

#endif
