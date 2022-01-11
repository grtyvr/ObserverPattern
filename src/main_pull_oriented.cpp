#include <iostream>
#include <list>

class Observer
{
public:
	virtual ~Observer(){};
	virtual void Update() = 0;
};

class Subject
{
public:
	virtual ~Subject(){};
	virtual void registerObserver(Observer *observer) = 0;
	virtual void removeObserver(Observer *observer) = 0;
	virtual void notifyObserver() = 0;
};

class DisplayElement
{
public:
	virtual ~DisplayElement(){};
	virtual void display() = 0;
};

class WeatherStation : public Subject
{
private:
	std::list<Observer *> observers_;
	float temperature;
	float humidity;
	float pressure;

public:
	virtual ~WeatherStation()
	{
		std::cout << "WeatherData shutting down." << std::endl;
	}

	void registerObserver(Observer *observer) override
	{
		std::cout << "Register new observer" << std::endl;
		observers_.push_back(observer);
	}
	void removeObserver(Observer *observer) override
	{
		std::cout << "Observer removed from the list." << std::endl;
		observers_.remove(observer);
	}
	void notifyObserver() override
	{
		std::list<Observer *>::iterator iterator = observers_.begin();
		HowManyObservers();
		std::cout << "Sending out the call..." << std::endl;
		while (iterator != observers_.end())
		{
			(*iterator)->Update();
			++iterator;
		}
	}
	void measurementsChanged()
	{
		notifyObserver();
	}

	void setMeasurements(float temperature, float humidity, float pressure)
	{
		// for testing purposes
		// ToDo: write to a webAPI like
		// std::cout << "Set new data" << std::endl;
		this->temperature = temperature;
		this->humidity = humidity;
		this->pressure = pressure;
		measurementsChanged();
	}

	void HowManyObservers()
	{
		std::cout << "there are " << observers_.size() << " observers currently." << std::endl;
	}
	float getTemperature()
	{
		return temperature;
	}
	float getHumidity()
	{
		return humidity;
	}
	float getPressure()
	{
		return pressure;
	}
};

class GenericObserver : public Observer, public DisplayElement
{
private:
	float temperature;
	float humidity;
	float pressure;
	WeatherStation &weatherStation_;
	static int static_number_;
	int number_;

public:
	GenericObserver(WeatherStation &weatherStation) : weatherStation_(weatherStation)
	{
		this->weatherStation_.registerObserver(this);
		std::cout << "Generic Display subscribing..." << ++GenericObserver::static_number_ << std::endl;
		this->number_ = GenericObserver::static_number_;
	}
	virtual ~GenericObserver()
	{
		std::cout << "Goodby" << std::endl;
	}
	void Update() override
	{
		this->temperature = weatherStation_.getTemperature();
		this->humidity = weatherStation_.getHumidity();
		this->pressure = weatherStation_.getPressure();
		display();
	}
	void RemoveMeFromTheList()
	{
		weatherStation_.removeObserver(this);
	}
	void display() override
	{ // display current measurements
		std::cout << "Generic Display:" << temperature << " F" << std::endl;
	}
};

class CurrentConditionsDisplay : public Observer, public DisplayElement
{
private:
	float temperature;
	float humidity;
	float pressure;
	WeatherStation &weatherStation_;

public:
	CurrentConditionsDisplay(WeatherStation &weatherStation) : weatherStation_(weatherStation)
	{
		this->weatherStation_.registerObserver(this);
		std::cout << "Current Conditions subscribing..." << std::endl;
	}
	virtual ~CurrentConditionsDisplay()
	{
		std::cout << "Goodby." << std::endl;
	}
	void Update() override
	{
		this->temperature = weatherStation_.getTemperature();
		this->humidity = weatherStation_.getHumidity();
		display();
	}
	void display() override
	{ // display current measurements
		std::cout << "Current Conditions:" << temperature << " F degress and "
				  << humidity << " humidity" << std::endl;
	}
	void RemoveMeFromTheList()
	{
		weatherStation_.removeObserver(this);
	}
};

class StatisticsDisplay : public Observer, public DisplayElement
{
private:
	float maxTemp = 0.0;
	float minTemp = 200;
	float tempSum = 0.0;
	float temp;
	int numReadings;
	WeatherStation &weatherStation_;

public:
	StatisticsDisplay(WeatherStation &weatherStation) : weatherStation_(weatherStation)
	{
		std::cout << "Statistics display subscribing..." << std::endl;
		this->weatherStation_.registerObserver(this);
	}
	void Update() override
	{
		temp = weatherStation_.getTemperature();
		tempSum += temp;
		numReadings += 1;
		if (temp > maxTemp)
		{
			maxTemp = temp;
		}
		if (temp < minTemp)
		{
			minTemp = temp;
		}
		display();
	}
	void display() override
	{ // display ave, min, max
		std::cout << "Average/Max/Min temperature:" << tempSum / numReadings << "/"
				  << maxTemp << "/" << minTemp << std::endl;
	}
	void RemoveMeFromTheList()
	{
		weatherStation_.removeObserver(this);
	}
};

class ForecastDisplay : public Observer, public DisplayElement
{
private:
	float pressure_;
	float currentPressure = 29.92;
	float lastPressure;
	WeatherStation &weatherStation_;

public:
	ForecastDisplay(WeatherStation &weatherStation) : weatherStation_(weatherStation)
	{
		std::cout << "Forecast display subscribing..." << std::endl;
		this->weatherStation_.registerObserver(this);
	}
	void Update() override
	{
		lastPressure = currentPressure;
		currentPressure = weatherStation_.getPressure();
		display();
	}
	void display() override
	{ // display forecast
		std::cout << "Forecast:";
		if (currentPressure > lastPressure)
		{
			std::cout << "Pressure rising --> good weather on the way.";
		}
		else if (currentPressure = lastPressure)
		{
			std::cout << "no changes forecast.";
		}
		else if (currentPressure < lastPressure)
		{
			std::cout << "Pressure falling --> a change is gonna come!";
		}
		std::cout << std::endl;
	}
	void RemoveMeFromTheList()
	{
		weatherStation_.removeObserver(this);
	}
};

class HeatIndexDisplay : public Observer, public DisplayElement
{
private:
	float temperature_;
	float humidity_;
	WeatherStation &weatherStation_;

public:
	HeatIndexDisplay(WeatherStation &weatherStation) : weatherStation_(weatherStation)
	{
		std::cout << "Heatindex display subscribing..." << std::endl;
		this->weatherStation_.registerObserver(this);
	}
	void Update() override
	{
		temperature_ = weatherStation_.getTemperature();
		humidity_ = weatherStation_.getHumidity();
		display();
	}
	void display() override
	{ // display forecast
		std::cout << "Heat Index: is " << Heatindex(temperature_, humidity_) << std::endl;
	}
	float Heatindex(float t, float rh)
	{
		float index = (float)((16.923 + (0.185212 * t) + (5.37941 * rh) - (0.100254 * t * rh) + (0.00941695 * (t * t)) + (0.00728898 * (rh * rh)) + (0.000345372 * (t * t * rh)) - (0.000814971 * (t * rh * rh)) +
							   (0.0000102102 * (t * t * rh * rh)) - (0.000038646 * (t * t * t)) + (0.0000291583 * (rh * rh * rh)) + (0.00000142721 * (t * t * t * rh)) +
							   (0.000000197483 * (t * rh * rh * rh)) - (0.0000000218429 * (t * t * t * rh * rh)) +
							   0.000000000843296 * (t * t * rh * rh * rh)) -
							  (0.0000000000481975 * (t * t * t * rh * rh * rh)));
		return index;
	}
	void RemoveMeFromTheList()
	{
		weatherStation_.removeObserver(this);
	}
};

int GenericObserver::static_number_ = 0;

int main(int argc, char *argv[])
{
	// start up our weather station
	WeatherStation *weatherStation = new WeatherStation;
	// register some observers
	GenericObserver *observer1 = new GenericObserver(*weatherStation);
	GenericObserver *observer2 = new GenericObserver(*weatherStation);
	GenericObserver *observer3 = new GenericObserver(*weatherStation);
	CurrentConditionsDisplay *currentConditions1 = new CurrentConditionsDisplay(*weatherStation);
	HeatIndexDisplay *heatIndexDisplay = new HeatIndexDisplay(*weatherStation);
	StatisticsDisplay *Statistics1 = new StatisticsDisplay(*weatherStation);
	// we can also declare observer variables an instantiate them with oberservers later.
	GenericObserver *observer4;
	ForecastDisplay *observer5;

	weatherStation->setMeasurements(80, 65, 30.4);
	observer3->RemoveMeFromTheList();
	observer5 = new ForecastDisplay(*weatherStation);

	weatherStation->setMeasurements(82, 70, 29.5);
	observer4 = new GenericObserver(*weatherStation);
	observer2->RemoveMeFromTheList();

	weatherStation->setMeasurements(78, 90, 29.2);
	observer5->RemoveMeFromTheList();

	weatherStation->setMeasurements(78, 90, 29.1);
	observer4->RemoveMeFromTheList();
	observer1->RemoveMeFromTheList();

	weatherStation->setMeasurements(78, 90, 29.2);
	currentConditions1->RemoveMeFromTheList();

	delete observer5;
	delete observer4;
	delete observer3;
	delete observer2;
	delete observer1;
	delete currentConditions1;
	delete heatIndexDisplay;
	delete weatherStation;
	return 0;
}
