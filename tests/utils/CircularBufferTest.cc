#include <gtest/gtest.h>

#include <utils/CircularBuffer.hh>

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

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

consteval bool ClearResetsState()
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.clear();
    buffer.push_back(7);
    return buffer.size() == 1 && buffer[0] == 7;
}
static_assert(ClearResetsState());

struct NoDefault
{
    int value;
    constexpr explicit NoDefault(int v) : value{v}
    {
    }
};

consteval bool StoresTypeWithoutDefaultConstructor()
{
    CircularBuffer<NoDefault, 3> buffer;
    buffer.emplace_back(7);
    return buffer[0].value == 7;
}
static_assert(StoresTypeWithoutDefaultConstructor());

static_assert(std::is_same_v<decltype(std::declval<CircularBuffer<int, 4> &>()[0]), int &>);
static_assert(std::is_same_v<decltype(std::declval<const CircularBuffer<int, 4> &>()[0]), const int &>);
static_assert(std::is_same_v<decltype(std::declval<CircularBuffer<int, 4> &&>()[0]), int &&>);

static_assert(std::is_trivially_destructible_v<CircularBuffer<int, 4>>);
static_assert(!std::is_trivially_destructible_v<CircularBuffer<std::string, 4>>);

struct Tracked
{
    static inline int alive = 0;
    static inline int constructed = 0;
    static inline int copied = 0;
    static inline int moved = 0;

    int value;

    explicit Tracked(int v) : value{v}
    {
        ++alive;
        ++constructed;
    }

    Tracked(const Tracked &other) : value{other.value}
    {
        ++alive;
        ++copied;
    }

    Tracked(Tracked &&other) noexcept : value{other.value}
    {
        ++alive;
        ++moved;
    }

    Tracked &operator=(const Tracked &) = delete;
    Tracked &operator=(Tracked &&) = delete;

    ~Tracked()
    {
        --alive;
    }

    static void Reset()
    {
        alive = 0;
        constructed = 0;
        copied = 0;
        moved = 0;
    }
};

struct ThrowOnDemand
{
    static inline int alive = 0;
    static inline bool armed = false;

    int value;

    explicit ThrowOnDemand(int v) : value{v}
    {
        if (armed)
        {
            throw std::runtime_error("construction failed");
        }
        ++alive;
    }

    ThrowOnDemand(const ThrowOnDemand &other) : value{other.value}
    {
        if (armed)
        {
            throw std::runtime_error("copy failed");
        }
        ++alive;
    }

    ThrowOnDemand(ThrowOnDemand &&other) noexcept : value{other.value}
    {
        ++alive;
    }

    ~ThrowOnDemand()
    {
        --alive;
    }

    static void Reset()
    {
        alive = 0;
        armed = false;
    }
};

TEST(CircularBufferTest, NewBufferIsEmpty)
{
    const CircularBuffer<int, 4> buffer;
    EXPECT_TRUE(buffer.empty());
    EXPECT_FALSE(buffer.full());
    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_EQ(buffer.capacity(), 4u);
}

TEST(CircularBufferTest, CapacityIsStatic)
{
    EXPECT_EQ((CircularBuffer<int, 8>::capacity()), 8u);
}

TEST(CircularBufferTest, IndexZeroIsNewest)
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(10);
    buffer.push_back(20);
    EXPECT_EQ(buffer[0], 20);
    EXPECT_EQ(buffer[1], 10);
}

TEST(CircularBufferTest, BecomesFullAtCapacity)
{
    CircularBuffer<int, 4> buffer;
    for (int i = 0; i < 3; ++i)
    {
        buffer.push_back(i);
    }
    EXPECT_FALSE(buffer.full());

    buffer.push_back(3);
    EXPECT_TRUE(buffer.full());
    EXPECT_EQ(buffer.size(), 4u);
}

TEST(CircularBufferTest, OverflowEvictsOldest)
{
    CircularBuffer<int, 4> buffer;
    for (int i = 1; i <= 6; ++i)
    {
        buffer.push_back(i);
    }

    EXPECT_EQ(buffer.size(), 4u);
    EXPECT_EQ(buffer[0], 6);
    EXPECT_EQ(buffer[1], 5);
    EXPECT_EQ(buffer[2], 4);
    EXPECT_EQ(buffer[3], 3);
}

TEST(CircularBufferTest, EmplaceBackReturnsReferenceToInserted)
{
    CircularBuffer<int, 4> buffer;
    int &inserted = buffer.emplace_back(42);
    EXPECT_EQ(inserted, 42);

    inserted = 43;
    EXPECT_EQ(buffer[0], 43);
}

TEST(CircularBufferTest, OperatorIndexAllowsModification)
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(1);
    buffer[0] = 99;
    EXPECT_EQ(buffer[0], 99);
}

TEST(CircularBufferTest, PopBackOnEmptyReturnsFalse)
{
    CircularBuffer<int, 4> buffer;
    EXPECT_FALSE(buffer.pop_back());
    EXPECT_TRUE(buffer.empty());
}

TEST(CircularBufferTest, PopBackRemovesNewest)
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(1);
    buffer.push_back(2);

    EXPECT_TRUE(buffer.pop_back());
    EXPECT_EQ(buffer.size(), 1u);
    EXPECT_EQ(buffer[0], 1);
}

TEST(CircularBufferTest, PopBackWorksAfterWrapAround)
{
    CircularBuffer<int, 4> buffer;
    for (int i = 1; i <= 6; ++i)
    {
        buffer.push_back(i);
    }

    EXPECT_TRUE(buffer.pop_back());
    EXPECT_EQ(buffer.size(), 3u);
    EXPECT_EQ(buffer[0], 5);
    EXPECT_EQ(buffer[2], 3);
}

TEST(CircularBufferTest, PushBackAfterPopBackReusesSlot)
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.pop_back();
    buffer.push_back(3);

    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_EQ(buffer[0], 3);
    EXPECT_EQ(buffer[1], 1);
}

TEST(CircularBufferTest, ClearEmptiesBuffer)
{
    CircularBuffer<int, 4> buffer;
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.clear();

    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0u);
}

TEST(CircularBufferTest, BufferIsReusableAfterClear)
{
    CircularBuffer<int, 4> buffer;
    for (int i = 1; i <= 6; ++i)
    {
        buffer.push_back(i);
    }
    buffer.clear();

    buffer.push_back(100);
    buffer.push_back(200);

    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_EQ(buffer[0], 200);
    EXPECT_EQ(buffer[1], 100);
}

class CircularBufferLifetime : public testing::Test
{
  protected:
    void SetUp() override
    {
        Tracked::Reset();
    }

    void TearDown() override
    {
        EXPECT_EQ(Tracked::alive, 0) << "elements outlived the buffer";
    }
};

TEST_F(CircularBufferLifetime, EmptyBufferConstructsNothing)
{
    const CircularBuffer<Tracked, 4> buffer;
    EXPECT_EQ(Tracked::constructed, 0);
    EXPECT_EQ(Tracked::alive, 0);
}

TEST_F(CircularBufferLifetime, DestructorDestroysElements)
{
    {
        CircularBuffer<Tracked, 4> buffer;
        buffer.emplace_back(1);
        buffer.emplace_back(2);
        EXPECT_EQ(Tracked::alive, 2);
    }
    EXPECT_EQ(Tracked::alive, 0);
}

TEST_F(CircularBufferLifetime, ClearDestroysElements)
{
    CircularBuffer<Tracked, 4> buffer;
    buffer.emplace_back(1);
    buffer.emplace_back(2);
    ASSERT_EQ(Tracked::alive, 2);

    buffer.clear();
    EXPECT_EQ(Tracked::alive, 0);
}

TEST_F(CircularBufferLifetime, ClearDestroysElementsAfterWrapAround)
{
    CircularBuffer<Tracked, 4> buffer;
    for (int i = 0; i < 6; ++i)
    {
        buffer.emplace_back(i);
    }
    ASSERT_EQ(Tracked::alive, 4);

    buffer.clear();
    EXPECT_EQ(Tracked::alive, 0);
}

TEST_F(CircularBufferLifetime, PopBackDestroysOneElement)
{
    CircularBuffer<Tracked, 4> buffer;
    buffer.emplace_back(1);
    buffer.emplace_back(2);

    EXPECT_TRUE(buffer.pop_back());
    EXPECT_EQ(Tracked::alive, 1);

    buffer.clear();
}

TEST_F(CircularBufferLifetime, OverflowDestroysEvictedElement)
{
    CircularBuffer<Tracked, 4> buffer;
    for (int i = 0; i < 6; ++i)
    {
        buffer.emplace_back(i);
    }

    EXPECT_EQ(Tracked::constructed, 6);
    EXPECT_EQ(Tracked::alive, 4);

    buffer.clear();
}

TEST_F(CircularBufferLifetime, EmplaceBackConstructsInPlace)
{
    CircularBuffer<Tracked, 4> buffer;
    buffer.emplace_back(1);

    EXPECT_EQ(Tracked::constructed, 1);
    EXPECT_EQ(Tracked::copied, 0) << "emplace_back must not copy";
    EXPECT_EQ(Tracked::moved, 0) << "emplace_back must not move";

    buffer.clear();
}

TEST_F(CircularBufferLifetime, PushBackOfRvalueMovesWithoutCopying)
{
    CircularBuffer<Tracked, 4> buffer;
    buffer.push_back(Tracked{1});

    EXPECT_EQ(Tracked::copied, 0) << "temporary must be moved, not copied";
    EXPECT_GE(Tracked::moved, 1);

    buffer.clear();
}

TEST_F(CircularBufferLifetime, PushBackOfLvalueCopiesOnce)
{
    CircularBuffer<Tracked, 4> buffer;
    Tracked source{1};
    buffer.push_back(source);

    EXPECT_EQ(Tracked::copied, 1) << "named object must be copied exactly once";
    EXPECT_EQ(Tracked::alive, 2) << "source must stay alive";

    buffer.clear();
}

class CircularBufferExceptions : public testing::Test
{
  protected:
    void SetUp() override
    {
        ThrowOnDemand::Reset();
    }

    void TearDown() override
    {
        ThrowOnDemand::Reset();
    }
};

TEST_F(CircularBufferExceptions, StateStaysConsistentWhenConstructorThrows)
{
    {
        CircularBuffer<ThrowOnDemand, 4> buffer;
        buffer.emplace_back(1);
        ASSERT_EQ(buffer.size(), 1u);

        ThrowOnDemand::armed = true;
        EXPECT_THROW((void)buffer.emplace_back(2), std::runtime_error);
        ThrowOnDemand::armed = false;

        EXPECT_EQ(buffer.size(), 1u) << "size() must not count an object that was never constructed";
        EXPECT_EQ(ThrowOnDemand::alive, 1);
        EXPECT_EQ(buffer[0].value, 1);
    }
    EXPECT_EQ(ThrowOnDemand::alive, 0);
}

TEST_F(CircularBufferExceptions, StateStaysConsistentWhenOverwritingThrows)
{
    {
        CircularBuffer<ThrowOnDemand, 2> buffer;
        buffer.emplace_back(1);
        buffer.emplace_back(2);
        ASSERT_TRUE(buffer.full());

        ThrowOnDemand::armed = true;
        EXPECT_THROW((void)buffer.emplace_back(3), std::runtime_error);
        ThrowOnDemand::armed = false;

        EXPECT_EQ(buffer.size(), 1u);
        EXPECT_EQ(ThrowOnDemand::alive, 1);
        EXPECT_EQ(buffer[0].value, 2) << "surviving element must remain reachable";
    }
    EXPECT_EQ(ThrowOnDemand::alive, 0);
}

template <typename T> T MakeValue(int seed);

template <> int MakeValue<int>(int seed)
{
    return seed;
}

template <> std::string MakeValue<std::string>(int seed)
{
    return std::string(static_cast<std::size_t>(seed), 'x');
}

template <> NoDefault MakeValue<NoDefault>(int seed)
{
    return NoDefault{seed};
}

template <typename T> class CircularBufferTyped : public testing::Test
{
};

using BufferTypes = testing::Types<int, std::string, NoDefault>;
TYPED_TEST_SUITE(CircularBufferTyped, BufferTypes);

TYPED_TEST(CircularBufferTyped, StoresAndReturnsValue)
{
    CircularBuffer<TypeParam, 3> buffer;
    buffer.emplace_back(MakeValue<TypeParam>(1));

    EXPECT_EQ(buffer.size(), 1u);
    EXPECT_FALSE(buffer.empty());
}

TYPED_TEST(CircularBufferTyped, ClearWorks)
{
    CircularBuffer<TypeParam, 3> buffer;
    buffer.emplace_back(MakeValue<TypeParam>(1));
    buffer.emplace_back(MakeValue<TypeParam>(2));
    buffer.clear();

    EXPECT_TRUE(buffer.empty());
}

TYPED_TEST(CircularBufferTyped, OverflowKeepsCapacityElements)
{
    CircularBuffer<TypeParam, 3> buffer;
    for (int i = 1; i <= 5; ++i)
    {
        buffer.emplace_back(MakeValue<TypeParam>(i));
    }

    EXPECT_EQ(buffer.size(), 3u);
    EXPECT_TRUE(buffer.full());
}

TEST(CircularBufferTest, StoresNonTrivialType)
{
    CircularBuffer<std::string, 3> buffer;
    buffer.emplace_back(1000, 'x');
    buffer.push_back("hello");

    EXPECT_EQ(buffer[0], "hello");
    EXPECT_EQ(buffer[1].size(), 1000u);
}

TEST(CircularBufferTest, StoresMoveOnlyType)
{
    CircularBuffer<std::unique_ptr<int>, 3> buffer;
    buffer.emplace_back(std::make_unique<int>(1));
    buffer.push_back(std::make_unique<int>(2));

    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_EQ(*buffer[0], 2);
    EXPECT_EQ(*buffer[1], 1);
}

TEST(CircularBufferTest, MoveOnlyTypeSurvivesOverflow)
{
    CircularBuffer<std::unique_ptr<int>, 2> buffer;
    for (int i = 1; i <= 4; ++i)
    {
        buffer.emplace_back(std::make_unique<int>(i));
    }

    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_EQ(*buffer[0], 4);
    EXPECT_EQ(*buffer[1], 3);
}

TEST(CircularBufferTest, StoresTypeWithoutDefaultConstructorAtRuntime)
{
    CircularBuffer<NoDefault, 3> buffer;
    buffer.emplace_back(7);
    buffer.emplace_back(8);

    EXPECT_EQ(buffer[0].value, 8);
    EXPECT_EQ(buffer[1].value, 7);
}

TEST(CircularBufferTest, CapacityOneAlwaysHoldsNewest)
{
    CircularBuffer<int, 1> buffer;
    buffer.push_back(1);
    EXPECT_TRUE(buffer.full());
    EXPECT_EQ(buffer[0], 1);

    buffer.push_back(2);
    EXPECT_EQ(buffer.size(), 1u);
    EXPECT_EQ(buffer[0], 2);
}

TEST(CircularBufferTest, ManyWrapAroundsKeepIndicesCorrect)
{
    CircularBuffer<int, 3> buffer;
    for (int i = 0; i < 100; ++i)
    {
        buffer.push_back(i);
    }

    EXPECT_EQ(buffer.size(), 3u);
    EXPECT_EQ(buffer[0], 99);
    EXPECT_EQ(buffer[1], 98);
    EXPECT_EQ(buffer[2], 97);
}

} // namespace
