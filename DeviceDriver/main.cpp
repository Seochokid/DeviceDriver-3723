#include <string>
#include "gmock/gmock.h"
#include "device_driver.h"
#include "application.h"

using namespace std;
using namespace testing;

class MockFlashMemoryDevice : public FlashMemoryDevice {
public:
	MOCK_METHOD(unsigned char, read, (long address), (override));
	MOCK_METHOD(void, write, (long address, unsigned char data), (override));
};

class DeviceDriverFixture : public Test {
public:
	MockFlashMemoryDevice MockHardware;
	DeviceDriver driver{ &MockHardware };
};

class ApplicationFixture : public Test {
public:
	MockFlashMemoryDevice MockHardware;
	DeviceDriver driver{ &MockHardware };
	Application app{ &driver };
};

TEST_F(DeviceDriverFixture, ReadMustOccurFiveTimes) {
	EXPECT_CALL(MockHardware, read)
		.Times(5);

	int data = driver.read(0xFF);
}

TEST_F(DeviceDriverFixture, ReadSuccess) {
	EXPECT_CALL(MockHardware, read(_))
		.WillRepeatedly(Return(1));

	int data = driver.read(0xFF);
	EXPECT_EQ(1, data);
}

TEST_F(DeviceDriverFixture, ReadFailWithException) {
	EXPECT_CALL(MockHardware, read(_))
		.WillOnce(Return(1))
		.WillRepeatedly(Return(2));

	try {
		int data = driver.read(0xFF);
		FAIL();
	}
	catch (std::exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "ReadFailException" });
	}
}

TEST_F(DeviceDriverFixture, WriteOccurMustOneRead) {
	EXPECT_CALL(MockHardware, read(_))
		.Times(1)
		.WillRepeatedly(Return(0xFF));

	EXPECT_CALL(MockHardware, write(_, _));

	driver.write(0xFF, 0xAB);
}

TEST_F(DeviceDriverFixture, WriteSuccess) {
	EXPECT_CALL(MockHardware, read(_))
		.Times(1)
		.WillRepeatedly(Return(0xFF));

	EXPECT_CALL(MockHardware, write(_, _));

	driver.write(0xFF, 0xAB);
}

TEST_F(DeviceDriverFixture, WriteFailWithException) {
	EXPECT_CALL(MockHardware, read(_))
		.Times(1)
		.WillRepeatedly(Return(0xAB));

	try {
		driver.write(0xFF, 0xAA);
		FAIL();
	}
	catch (std::exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "WriteFailException" });
	}
}

TEST_F(ApplicationFixture, ReadAndPrintCallRead25Times) {
	EXPECT_CALL(MockHardware, read(_))
		.Times(25)
		.WillRepeatedly(Return(0xAB));
	app.readAndPrint(0, 5);
}

TEST_F(ApplicationFixture, ReadAndPrintSuccess) {
	EXPECT_CALL(MockHardware, read(_))
		.WillRepeatedly(Return(0xCD));

	stringstream buffer;
	streambuf* buf = cout.rdbuf(buffer.rdbuf());

	app.readAndPrint(0, 5);

	cout.rdbuf(buf);

	string expected;
	for (long addr = 0; addr < 5; ++addr) {
		expected += "Address: " + to_string(addr) + ", Data: " + to_string(0xCD) + "\n";
	}
	
	EXPECT_EQ(buffer.str(), expected);
}

TEST_F(ApplicationFixture, WriteAllCallSuccessWithWrite5Times) {
	EXPECT_CALL(MockHardware, read(_))
		.Times(5)
		.WillRepeatedly(Return(0xFF));

	EXPECT_CALL(MockHardware, write(_, _))
		.Times(5);
	
	app.writeAll(0xAB);
}

TEST_F(ApplicationFixture, WriteAllFailWithException) {
	EXPECT_CALL(MockHardware, read(_))
		.WillOnce(Return(0xAB))
		.WillRepeatedly(Return(0xFF));

	try {
		app.writeAll(0xAB);
		FAIL();
	}
	catch (std::exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "WriteFailException" });
	}
}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}