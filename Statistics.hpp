#ifndef ANTS_STATISTICS_HPP
#define ANTS_STATISTICS_HPP

#include <memory>
#include <vector>
#include <functional>

class Statistics
{
	void UpdateFoodRemaining(int delta);
	void UpdateFoodDelivered(int delta);
	void UpdateAntsAmount(int delta);

private:
	int m_foodRemaining;
	int m_foodDelivered;
	int m_antsAmount;
};

class FoodSubject
{
public:
	void FoodCreated() {}

	void FoodTaken() {}

	void FoodDepleted() {}

	void FoodDelivered() {}
};


#endif //ANTS_STATISTICS_HPP
