#ifndef WXSTREAMBUF_H
#define WXSTREAMBUF_H

#include <FileIO/Dynamic/StreamBuf.h>

class wxStreamBuf : public StreamBuf
{
public:
	wxStreamBuf(bool isBE) : StreamBuf(isBE) {		
	}
	
	wxStreamBuf(StreamBuf *pParent) : StreamBuf(pParent) {		
	}

	// made this non virtual as we rely on static_cast internally
	~wxStreamBuf() {
		m_buffer.Free();
	}
	
	void Read(wxInputStream* buffer, wxFileOffset size);
	
	void Write(wxOutputStream *buffer) const;
};
#endif
