//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//
#if !defined( INETCHANNELINFO_H )
#define INETCHANNELINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "globalvars_base.h"
#include <vector>
typedef unsigned char byte;

class	IDemoRecorder;
class	INetMessage;
class	INetChannelHandler;
typedef struct netpacket_s netpacket_t;
typedef struct netadr_s	netadr_t;
typedef void* FileHandle_t;
class bf_write;
class bf_read;

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName(void) const = 0;	// get channel name
	virtual const char* GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};

class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel(void) {};

	virtual void	SetDataRate(float rate) = 0;
	virtual bool	RegisterMessage(INetMessage* msg) = 0;
	virtual bool	StartStreaming(unsigned int challengeNr) = 0;
	virtual void	ResetStreaming(void) = 0;
	virtual void	SetTimeout(float seconds) = 0;
	virtual void	SetDemoRecorder(IDemoRecorder* recorder) = 0;
	virtual void	SetChallengeNr(unsigned int chnr) = 0;

	virtual void	Reset(void) = 0;
	virtual void	Clear(void) = 0;
	virtual void	Shutdown(const char* reason) = 0;

	virtual void	ProcessPlayback(void) = 0;
	virtual bool	ProcessStream(void) = 0;
	virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	virtual bool	SendNetMsg(INetMessage& msg, bool bForceReliable = false, bool bVoice = false) = 0;
#ifdef POSIX
	FORCEINLINE bool SendNetMsg(INetMessage const& msg, bool bForceReliable = false, bool bVoice = false) { return SendNetMsg(*((INetMessage*)&msg), bForceReliable, bVoice); }
#endif
	virtual bool	SendData(bf_write& msg, bool bReliable = true) = 0;
	virtual bool	SendFile(const char* filename, unsigned int transferID) = 0;
	virtual void	DenyFile(const char* filename, unsigned int transferID) = 0;
	virtual void	RequestFile_OLD(const char* filename, unsigned int transferID) = 0;	// get rid of this function when we version the 
	virtual void	SetChoked(void) = 0;
	virtual int		SendDatagram(bf_write* data) = 0;
	virtual bool	Transmit(bool onlyReliable = false) = 0;

	virtual const netadr_t& GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler* GetMsgHandler(void) const = 0;
	virtual int				GetDropNumber(void) const = 0;
	virtual int				GetSocket(void) const = 0;
	virtual unsigned int	GetChallengeNr(void) const = 0;
	virtual void			GetSequenceData(int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck) = 0;
	virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void	UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool	CanPacket(void) const = 0;
	virtual bool	IsOverflowed(void) const = 0;
	virtual bool	IsTimedOut(void) const = 0;
	virtual bool	HasPendingReliableData(void) = 0;

	virtual void	SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void	SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char* filename) = 0;
	virtual float	GetTimeSinceLastReceived(void) const = 0;	// get time since last received packet in seconds

	virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;

	virtual bool	IsNull() const = 0;
	virtual int		GetNumBitsWritten(bool bReliable) = 0;
	virtual void	SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int		GetMaxRoutablePayloadSize() = 0;

	virtual int		GetProtocolVersion() = 0;
};

// How fast to converge flow estimates
#define FLOW_AVG ( 3.0 / 4.0 )
 // Don't compute more often than this
#define FLOW_INTERVAL 0.25


#define NET_FRAMES_BACKUP	64		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits

#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel

class CNetChan : public INetChannel
{

private: // netchan structurs

	typedef struct dataFragments_s
	{
		FileHandle_t	file;			// open file handle
		char			filename[260]; // filename
		char* buffer;			// if NULL it's a file
		unsigned int	bytes;			// size in bytes
		unsigned int	bits;			// size in bits
		unsigned int	transferID;		// only for files
		bool			isCompressed;	// true if data is bzip compressed
		unsigned int	nUncompressedSize; // full size in bytes
		bool			asTCP;			// send as TCP stream
		int				numFragments;	// number of total fragments
		int				ackedFragments; // number of fragments send & acknowledged
		int				pendingFragments; // number of fragments send, but not acknowledged yet
	} dataFragments_t;

	struct subChannel_s
	{
		int				startFraggment[2];
		int				numFragments[2];
		int				sendSeqNr;
		int				state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
		int				index; // index in m_SubChannels[]

		void Free()
		{
			state = SUBCHANNEL_FREE;
			sendSeqNr = -1;
			for (int i = 0; i < 2; i++)
			{
				numFragments[i] = 0;
				startFraggment[i] = -1;
			}
		}
	};

	// Client's now store the command they sent to the server and the entire results set of
	//  that command. 
	typedef struct netframe_s
	{
		// Data received from server
		float			time;			// net_time received/send
		int				size;			// total size in bytes
		float			latency;		// raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
		float			avg_latency;	// averaged ping for this packet
		bool			valid;			// false if dropped, lost, flushed
		int				choked;			// number of previously chocked packets
		int				dropped;
		float			m_flInterpolationAmount;
		unsigned short	msggroups[INetChannelInfo::TOTAL];	// received bytes for each message group
	} netframe_t;

	typedef struct
	{
		float		nextcompute;	// Time when we should recompute k/sec data
		float		avgbytespersec;	// average bytes/sec
		float		avgpacketspersec;// average packets/sec
		float		avgloss;		// average packet loss [0..1]
		float		avgchoke;		// average packet choke [0..1]
		float		avglatency;		// average ping, not cleaned
		float		latency;		// current ping, more accurate also more jittering
		int			totalpackets;	// total processed packets
		int			totalbytes;		// total processed bytes
		int			currentindex;		// current frame index
		netframe_t	frames[NET_FRAMES_BACKUP]; // frame history
		netframe_t* currentframe;	// current frame
	} netflow_t;

public:

	bool		m_bProcessingMessages;
	bool		m_bClearedDuringProcessing;
	bool		m_bShouldDelete;

	// last send outgoing sequence number
	int			m_nOutSequenceNr;
	// last received incoming sequnec number
	int			m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int			m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int			m_nOutReliableState;
	// state of incoming reliable data
	int			m_nInReliableState;

	int			m_nChokedPackets;	//number of choked packets


	// Reliable data buffer, send which each packet (or put in waiting list)
	void*	m_StreamReliable;
	std::vector<byte> m_ReliableDataBuffer;

	// unreliable message buffer, cleared which each packet
	void*	m_StreamUnreliable;
	std::vector<byte> m_UnreliableDataBuffer;

	void*	m_StreamVoice;
	std::vector<byte> m_VoiceDataBuffer;

	// don't use any vars below this (only in net_ws.cpp)

	int			m_Socket;   // NS_SERVER or NS_CLIENT index, depending on channel.
	int			m_StreamSocket;	// TCP socket handle

	unsigned int m_MaxReliablePayloadSize;	// max size of reliable payload in a single packet	

	// Address this channel is talking to.
	void*	remote_address;

	// For timeouts.  Time last message was received.
	float		last_received;
	// Time when channel was connected.
	double      connect_time;

	// Bandwidth choke
	// Bytes per second
	int			m_Rate;
	// If realtime > cleartime, free to send next packet
	double		m_fClearTime;

	std::vector<dataFragments_t*> m_WaitingList[2];	// waiting list for reliable data and file transfer
	dataFragments_t					m_ReceiveList[2]; // receive buffers for streams
	subChannel_s					m_SubChannels[MAX_SUBCHANNELS];

	unsigned int	m_FileRequestCounter;	// increasing counter with each file request
	bool			m_bFileBackgroundTranmission; // if true, only send 1 fragment per packet
	bool			m_bUseCompression;	// if true, larger reliable data will be bzip compressed

	// TCP stream state maschine:
	bool		m_StreamActive;		// true if TCP is active
	int			m_SteamType;		// STREAM_CMD_*
	int			m_StreamSeqNr;		// each blob send of TCP as an increasing ID
	int			m_StreamLength;		// total length of current stream blob
	int			m_StreamReceived;	// length of already received bytes
	char		m_SteamFile[260];	// if receiving file, this is it's name
	std::vector<byte> m_StreamData;			// Here goes the stream data (if not file). Only allocated if we're going to use it.

	// packet history
	netflow_t		m_DataFlow[MAX_FLOWS];
	int				m_MsgStats[INetChannelInfo::TOTAL];	// total bytes for each message group


	int				m_PacketDrop;	// packets lost before getting last update (was global net_drop)

	char			m_Name[32];		// channel name

	unsigned int	m_ChallengeNr;	// unique, random challenge number 

	float		m_Timeout;		// in seconds 

	INetChannelHandler* m_MessageHandler;	// who registers and processes messages
	std::vector<INetMessage*> m_NetMessages;		// list of registered message
	IDemoRecorder* m_DemoRecorder;			// if != NULL points to a recording/playback demo object
	int							m_nQueuedPackets;

	float						m_flInterpolationAmount;
	float						m_flRemoteFrameTime;
	float						m_flRemoteFrameTimeStdDeviation;
	int							m_nMaxRoutablePayloadSize;

	int							m_nSplitPacketSequence;
	bool						m_bStreamContainsChallenge;  // true if PACKET_FLAG_CHALLENGE was set when receiving packets from the sender

	int							m_nProtocolVersion;		// PROTOCOL_VERSION if we're not playing a demo - otherwise, whatever was in the demo header's networkprotocol if the CNetChan instance was created by a demo player.
};

#endif // INETCHANNEL_H