#include <iostream>
#include <list>

class IObserver
{
public:
	virtual ~IObserver(){};
	virtual void Update(float temp, float humidity, float pressure) = 0;
};

class ISubject
{
public:
	virtual ~ISubject(){};
	virtual void Attach(IObserver *observer) = 0;
	virtual void Detach(IObserver *observer) = 0;
	virtual void Notify() = 0;
};

class DisplayElement
{
public:
	virtual ~DisplayElement(){};
	virtual void display() = 0;
};

class Subject : public ISubject
{
private:
	std::list<IObserver *> observers_;
	float temperature;
	float humidity;
	float pressure;

public:
	virtual ~Subject()
	{
		std::cout << "WeatherData leaving." << std::endl;
	}

	void Attach(IObserver *observer) override
	{
		// std::cout << "Register new observer" << std::endl;
		observers_.push_back(observer);
	}
	void Detach(IObserver *observer) override
	{
		std::cout << "Signing out...." << std::endl;
		observers_.remove(observer);
	}
	void Notify() override
	{
		std::list<IObserver *>::iterator iterator = observers_.begin();
		HowManyObservers();
		std::cout << "Sending out the call..." << std::endl;
		while (iterator != observers_.end())
		{
			(*iterator)->Update(temperature, humidity, pressure);
			++iterator;
		}
	}
	void measurementsChanged()
	{
		Notify();
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
		std::cout << "there are " << observers_.size() << " observers currently.\n"
				  << std::endl;
	}
};

class Observer : public IObserver, public DisplayElement
{
private:
	float temperature;
	float humidity;
	float pressure;
	Subject &subject_;
	static int static_number_;
	int number_;

public:
	Observer(Subject &subject) : subject_(subject)
	{
		this->subject_.Attach(this);
		std::cout << "Generic subscriber ..." << ++Observer::static_number_ << std::endl;
		this->number_ = Observer::static_number_;
	}
	virtual ~Observer()
	{
		std::cout << "Unsubscribe! was subscriber " << std::endl;
	}
	void Update(float temperature, float humidity, float pressure) override
	{
		this->temperature = temperature;
		this->humidity = humidity;
		display();
	}
	void RemoveMeFromTheList() {
    subject_.Detach(this);
    std::cout << "Observer \"" << number_ << "\" removed from the list.\n";
  	}
	void display() override
	{ // display current measurements
		std::cout << "Generic Display:" << temperature << " F" << std::endl;
	}
};


class CurrentConditionsDisplay : public IObserver, public DisplayElement {
	private:
		float temperature;
		float humidity;
		float pressure;
		Subject &subject_;
		static int static_number_;
  		int number_;

	public:
	CurrentConditionsDisplay(Subject &subject) : subject_(subject){
		this->subject_.Attach(this);
		std::cout << "Current Conditions subscriber ..." << ++CurrentConditionsDisplay::static_number_ << std::endl;
		this->number_ = CurrentConditionsDisplay::static_number_;
	}
	virtual ~CurrentConditionsDisplay() {
		std::cout << "Unsubscribe! was subscriber " << std::endl;
	}
	void Update(float temperature, float humidity, float pressure) override {
		this->temperature = temperature;
		this->humidity = humidity;
		display();
	}
	void display() override { // display current measurements
		std::cout << "Current Conditions:" << temperature << " F degress and "
						<< humidity << " humidity" << std::endl;
	}
	void RemoveMeFromTheList() {
    subject_.Detach(this);
    std::cout << "Observer \"" << number_ << "\" removed from the list.\n";
  	}
};

class StatisticsDisplay : public IObserver, public DisplayElement {
	private:
		float maxTemp = 0.0;
		float minTemp = 200;
		float tempSum = 0.0;
		int numReadings;
		Subject &subject_;
	public:
	StatisticsDisplay(Subject &subject) : subject_(subject) {
		std::cout << "Statistics display subscribing..." << std::endl;
		this->subject_.Attach(this);
	}
	void Update(float temperature, float humidity, float presure) override {
		tempSum += temperature;
		numReadings += 1;
		if (temperature > maxTemp){
			maxTemp = temperature;
		}
		if (temperature < minTemp){
			minTemp = temperature;
		}
		display();
	}
	void display() override { // display ave, min, max
		std::cout << "Average/Max/Min temperature:" << tempSum/numReadings << "/"
						<< maxTemp << "/" << minTemp << std::endl;

	}
};

class ForecastDisplay : public IObserver, public DisplayElement {
	private:
		float currentPressure = 29.92;
		float lastPressure;
		Subject &subject_;
	public:
	ForecastDisplay(Subject &subject) : subject_(subject){
		std::cout << "Forecast display subscribing..." << std::endl;
		this->subject_.Attach(this);
	}
	void Update(float temperature, float humidity, float pressure) override {
		lastPressure = currentPressure;
		currentPressure = pressure;
		display();
	}
	void display() override { // display forecast
		std::cout << "Forecast:";
		if (currentPressure > lastPressure) {
			std::cout << "Pressure rising --> good weather on the way.";
		} else if (currentPressure = lastPressure) {
			std::cout << "no changes forecast.";
		} else if (currentPressure < lastPressure) {
			std::cout << "Pressure falling --> a change is gonna come!";
		}
		std::cout << std::endl;
	}
};

int Observer::static_number_ = 0;
int CurrentConditionsDisplay::static_number_ = 0;

void ClientCode()
{
	Subject *subject = new Subject;
	Observer *observer1 = new Observer(*subject);
	Observer *observer2 = new Observer(*subject);
	Observer *observer3 = new Observer(*subject);
	CurrentConditionsDisplay *CurrentConditions1 = new CurrentConditionsDisplay(*subject);
	StatisticsDisplay *Statistics1 = new StatisticsDisplay(*subject);
	Observer *observer4;
	Observer *observer5;

	subject->setMeasurements(80, 65, 30.4);

	observer3->RemoveMeFromTheList();

	subject->setMeasurements(82, 70, 29.5);

	observer4 = new Observer(*subject);

	observer2->RemoveMeFromTheList();
	observer5 = new Observer(*subject);

	subject->setMeasurements(78, 90, 29.2);

	observer5->RemoveMeFromTheList();

	subject->setMeasurements(78, 90, 29.1);
	observer4->RemoveMeFromTheList();
	observer1->RemoveMeFromTheList();
	subject->setMeasurements(78, 90, 29.2);
	CurrentConditions1->RemoveMeFromTheList();

	delete observer5;
	delete observer4;
	delete observer3;
	delete observer2;
	delete observer1;
	delete subject;
}

int main(int argc, char *argv[])
{
	ClientCode();
	return 0;
	// std::cout << "Starting Weather Station simulation" << std::endl;
	// Subject *subject = new Subject();
	// std::cout << "subcriptions coming..." << std::endl;
	// Observer *observer1 = new Observer(*subject);
	// std::cout << "second one..." << std::endl;
	// CurrentConditionsDisplay *currentConditionsDisplay2 = new CurrentConditionsDisplay(*weatherData);
	// //	StatisticsDisplay *statisticsDisplay = new StatisticsDisplay(*weatherData);
	// //	ForecastDisplay *forecastDisplay = new ForecastDisplay(*weatherData);

	// weatherData->setMeasurements(80, 65, 30.4);
	// weatherData->setMeasurements(82, 70, 29.5);
	// weatherData->setMeasurements(78, 90, 29.2);

	// delete currentConditionsDisplay1;
	// delete currentConditionsDisplay2;
	// //	delete statisticsDisplay;
	// //	delete forecastDisplay;
	// delete weatherData;
}