#include <gtest/gtest.h>
#include <pystruct.h>
using namespace std;

TEST(PyCode, PyStructPack){
    Py::Initialize();

    stringstream ss;

    ss << 321 << ", " << 1337;
    string res = pystruct::pack("<II", ss);
    ASSERT_EQ(res, "b\'A\\x01\\x00\\x009\\x05\\x00\\x00\'");
    ss.clear();
    ss << "bzz" << ", " << 1337 << ", " << 321;
    res = pystruct::pack("<3sII", ss);
    ASSERT_EQ(res,"b\'bzz9\\x05\\x00\\x00A\\x01\\x00\\x00\'");

    Py::Finalize();
}