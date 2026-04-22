#include <iostream>
#include <stdexcept>
#include <cstring>

#pragma warning(disable:4996)


class Socket;

class Device 
{
public:
	Device()
		:Device("Empty", "Unknown", 1)
	{ }

	Device(const char* description, const char* brand, unsigned power)
		:id(nextId++)
	{
		setBrand(brand);
		try
		{
			setDescription(description);
		}
		catch (...)
		{
			delete[] brand;
			throw;
		}
		setPower(power);
	}

	Device(const Device& other)
		:Device(other.descr, other.brand, other.power)
	{}

	Device& operator=(const Device& other)
	{
		if (this != &other)
		{
			char* newBrand;
			char* newDescr;

			int len = strlen(other.descr);
			newDescr = new char[len + 1] {};
			len = strlen(other.brand);
			try 
			{
				newBrand = new char[len + 1] {};
			}
			catch (...)
			{
				delete[] newDescr;
				throw;
			}
			plugOut();
			strcpy(newBrand, other.brand);
			strcpy(newDescr, other.descr);
			delete[] brand;
			brand = newBrand;
			delete[] descr;
			descr = newDescr;

			power = other.power;
		}
		return *this;
	}

	~Device()
	{
		delete[] descr;
		delete[] brand;
		plugOut();
	}

	bool isPluggedIn() { return socket != nullptr; }
	const char* getDescription() const { return descr; }
	const char* getBrand() const { return brand; }
	unsigned getPower() const { return power; }
	bool isTurnedOn() const { return turnedOn; }
	unsigned getId() const { return id; }

	void setDescription(const char* newDescr) 
	{ 
		char* copy = getDynCopy(newDescr);
		strcpy(copy, newDescr);
		delete[] descr;
		descr = copy;
	}

	void setBrand(const char* newBrand) 
	{ 
		char* copy = getDynCopy(newBrand);
		strcpy(copy, newBrand);
		delete[] brand;
		brand = copy;
	}

	void setPower(unsigned newPower)
	{
		if (!isPluggedIn() || newPower == 0) throw std::runtime_error("error");
		power = newPower;
	}

	void plugIn(Socket* newSocket)
	{
		if (!newSocket || isPluggedIn()) throw std::runtime_error("neshto");
		if (socket == newSocket) return;
		socket = newSocket;
		try 
		{
			socket->plugIn(this);
		}
		catch (...)
		{
			socket = nullptr;
		}
	}
	void plugOut()
	{
		if (!isPluggedIn()) throw std::runtime_error("Sasa");
		turnOff();
		socket->plugOut(this);
		socket = nullptr;
	}
	void turnOn()
	{
		if (!isPluggedIn() || isTurnedOn()) throw std::runtime_error("sasa");
		turnedOn = true;
		socket->notifyPowerChange();
	}
	void turnOff()
	{
		if (!isTurnedOn()) throw std::runtime_error("sasa");
		turnedOn = false;
		socket->notifyPowerChange();
	}

private:
	char* descr = nullptr;
	char* brand = nullptr;
	unsigned power;
	Socket* socket = nullptr;
	bool turnedOn = false;
	const unsigned id;

	static unsigned nextId;

	bool isValidStr(const char* str)
	{
		//if (!str) throw std::runtime_error("bad");
		return strlen(str) != 0;
	}

	char* getDynCopy(const char* str)
	{
		if (!str || !isValidStr(str)) throw std::invalid_argument("invalid");
		int len = strlen(str);
		char* copy = new char[len + 1] {};
		strcpy(copy, str);
		return copy;
	}
};

unsigned Device::nextId = 0;



class Socket
{
public:
	Socket(unsigned size, unsigned maxPower)
		:size(size), maxPower(maxPower)
	{
		devices = new Device * [size] {};
		for (int i = 0; i < size; i++)
		{
			if (devices[i] != nullptr) {
				devices[i] = nullptr;
			}		
		}
	}


	Socket(const Socket& other)
		:Socket(other.size, other.maxPower)
	{
		if (size < other.size) throw std::runtime_error("cant...");
	}

	Socket& operator=(const Socket& other)
	{
		if (size < other.size) throw std::runtime_error("cant...");
		if(this != &other)
		{
			maxPower = other.maxPower;

		}
		return *this;
	}

	~Socket()
	{
		delete[] devices;
	}

	void plugIn(Device* device)
	{
		for (int i = 0; i < size; i++)
		{
			if (devices[i] == nullptr)
			{
				devices[i] = device;
				return;
			}
		}
		throw std::runtime_error("no more space");
	}

	void plugOut(Device* device)
	{

	}

	unsigned getCurrPower()
	{
		unsigned sum;
		for (int i = 0; i < size; i++)
		{
			if (devices[i] != nullptr) {
				if (devices[i]->isTurnedOn())
				{
					sum += devices[i]->getPower();
				}
			}
		}
		return sum;
	}

	void notifyPowerChange()
	{
		unsigned currPower = getCurrPower();
		if (currPower > maxPower)
		{
			burned = true;
			unplugAll();
		}
	}

private:
	Device** devices;
	const unsigned size;
	bool burned = false;

	unsigned maxPower;

	void unplugAll()
	{
		for (int i = 0; i < size; i++)
		{
			if (devices[i] != nullptr)
			{
				devices[i]->plugOut();
			}
		}
	}
};

int main()
{
	
}