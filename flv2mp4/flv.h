#ifndef FLV_H
#define FLV_H

class Flv
{
public:
	Flv( int flvFd );
	~Flv();
public:
	int GetFirstDataOffset();
	int GetFirstVideoDataOffset();
	int GetFirstAudioDataOffset();
	int GetVideoTagHeaderSize();
	int GetAudioTagHeaerSize();
private:
	int AnalyFlvHeader();
	int AudioTag();
	int VideoTag();
	int AnalyFlvTag();
private:
	int mFlvFd;
	int mFirstVideoOffset;
	int mFirstAudioOffset;
	int mFirstDataOffset;

	char mhasVideo;
	char mHasAuddio;
	char mCodecID;
	char mSoundFormat;
	char mTemp[11];
	//how much video or Audio we has analy.
	char mAnaylyVideoTagNum;
	char mAnaylyAudioTagNum;
	int mPos;
};

#endif
