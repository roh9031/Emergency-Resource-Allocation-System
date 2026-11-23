/* Scale factor that turns real-valued weights into integer flow costs
 FlowNetwork costs are long long, so every double that becomes a cost
 (site urgency, alpha * travel minutes) is multiplied by kCostScale and
 rounded once with std::llround, for example
     long long siteCost = -std::llround(urgency * kCostScale);
     long long linkCost = std::llround(alpha * minutes * kCostScale);
 All costs are scaled by the same factor, so their ratios are kept up to
 rounding at 3 decimal places. Results report the real (unscaled) values */
#pragma once

inline constexpr long long kCostScale = 1000;
