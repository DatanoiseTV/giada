#include <memory>
#include "../src/core/const.h"
#include "../src/core/wave.h"
#include "../src/core/waveFx.h"
#include "../src/core/types.h"
#include <catch2/catch.hpp>


using namespace giada;
using namespace giada::m;


TEST_CASE("waveFx")
{
	static const int SAMPLE_RATE    = 44100;
	static const int BUFFER_SIZE    = 4000;
	static const int BIT_DEPTH      = 32;

	Wave waveMono(0);
	Wave waveStereo(0);

	waveMono.alloc(BUFFER_SIZE, 1, SAMPLE_RATE, BIT_DEPTH, "path/to/sample-mono.wav");
	waveStereo.alloc(BUFFER_SIZE, 2, SAMPLE_RATE, BIT_DEPTH, "path/to/sample-stereo.wav");

	SECTION("test mono->stereo conversion")
	{
		int prevSize = waveMono.getSize();

		REQUIRE(wfx::monoToStereo(waveMono) == G_RES_OK);
		REQUIRE(waveMono.getSize() == prevSize);  // size does not change, channels do
		REQUIRE(waveMono.getChannels() == 2);

		SECTION("test mono->stereo conversion for already stereo wave")
		{
			/* Should do nothing. */
			int prevSize = waveStereo.getSize();

			REQUIRE(wfx::monoToStereo(waveStereo) == G_RES_OK);
			REQUIRE(waveStereo.getSize() == prevSize);
			REQUIRE(waveStereo.getChannels() == 2);
		}
	}

	SECTION("test silence")
	{
		int a = 20;
		int b = 57;
		wfx::silence(waveStereo, a, b);

		for (int i=a; i<b; i++)
			for (int k=0; k<waveStereo.getChannels(); k++)
				REQUIRE(waveStereo[i][k] == 0.0f);
	}

	SECTION("test cut")
	{
		int a = 47;
		int b = 210;
		int range = b - a;
		int prevSize = waveStereo.getSize();

		wfx::cut(waveStereo, a, b);

		REQUIRE(waveStereo.getSize() == prevSize - range);
	}

	SECTION("test trim")
	{
		int a = 47;
		int b = 210;
		int area = b - a;

		wfx::trim(waveStereo, a, b);

		REQUIRE(waveStereo.getSize() == area);
	}

	SECTION("test fade")
	{
		int a = 47;
		int b = 500;

		wfx::fade(waveStereo, a, b, wfx::Fade::IN);
		wfx::fade(waveStereo, a, b, wfx::Fade::OUT);

		REQUIRE(waveStereo.getFrame(a)[0] == 0.0f);
		REQUIRE(waveStereo.getFrame(a)[1] == 0.0f);
		REQUIRE(waveStereo.getFrame(b)[0] == 0.0f);
		REQUIRE(waveStereo.getFrame(b)[1] == 0.0f);
	}

	SECTION("test smooth")
	{
		int a = 11;
		int b = 79;

		wfx::smooth(waveStereo, a, b);

		REQUIRE(waveStereo.getFrame(a)[0] == 0.0f);
		REQUIRE(waveStereo.getFrame(a)[1] == 0.0f);
		REQUIRE(waveStereo.getFrame(b)[0] == 0.0f);
		REQUIRE(waveStereo.getFrame(b)[1] == 0.0f);
	}
}
