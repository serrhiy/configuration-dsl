#include <gtest/gtest.h>

#include <utils/CircularBuffer.hh>

#include <string>

namespace
{

using utils::CircularBuffer;

consteval bool OverflowKeepsNewest()
{
    CircularBuffer<int, 4> buffer;
    for (int i = 1; i <= 6; ++i)
    {
        buffer.push_back(i);
    }
    return buffer.size() == 4 && buffer[0] == 6 && buffer[3] == 3;
}
static_assert(OverflowKeepsNewest());

TEST(CircularBufferTest, NewBufferIsEmpty)
{
    const CircularBuffer<int, 4> buffer;
    EXPECT_TRUE(buffer.empty());
    EXPECT_FALSE(buffer.full());
    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_EQ(buffer.capacity(), 4u);
}

TEST(CircularBufferTest, IndexZeroIsNewest)
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(10);
    buffer.push_back(20);
    EXPECT_EQ(buffer[0], 20);
    EXPECT_EQ(buffer[1], 10);
}

TEST(CircularBufferTest, PopBackOnEmptyReturnsFalse)
{
    CircularBuffer<int, 4> buffer;
    EXPECT_FALSE(buffer.pop_back());
}

template <typename T> class CircularBufferTyped : public testing::Test
{
};

using BufferTypes = testing::Types<int, std::string>;
TYPED_TEST_SUITE(CircularBufferTyped, BufferTypes);

TYPED_TEST(CircularBufferTyped, StoresValue)
{
    CircularBuffer<TypeParam, 3> buffer;
    buffer.push_back(TypeParam{});
    EXPECT_EQ(buffer.size(), 1u);
}

} // namespace