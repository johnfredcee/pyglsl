
#include <StdAfx.h>
#include <wx/stream.h>
#include "wxStreamBuf.h"

void wxStreamBuf::Read(wxInputStream* stream, wxFileOffset size)
{
	if (m_pParent)
	{
		static_cast<wxStreamBuf*>(m_pParent)->Read(stream, size);
	}
	else if (stream)
	{
		if (size == 0)
		{
			// not sure why we do this..? .. to calc size?
			wxFileOffset pos = stream->TellI();
			stream->SeekI(0, wxFromEnd);
			size = stream->TellI() - pos;
			stream->SeekI(pos, wxFromStart);
		}
	    wxFileOffset streamPos = wxFileOffset(m_buffer.GetCount());
		wxFileOffset oldStreamSize = wxFileOffset(m_curPos);
		m_buffer.Resize(aqua::Uint32(oldStreamSize + size));
		if (m_buffer.GetCount() == oldStreamSize + size)
		{
			stream->Read(&m_buffer[aqua::Uint32(streamPos)], aqua::Uint32(size));			
			size_t amountRead = stream->LastRead();
			if (amountRead != size)
			{
				m_buffer.Resize(aqua::Uint32(oldStreamSize + amountRead));
			}
			m_curPos = aqua::Uint32(oldStreamSize + amountRead);
			_CheckWritten();
		}
	}
}

void wxStreamBuf::Write(wxOutputStream *stream) const
{
	if (m_pParent)
	{
		static_cast<wxStreamBuf*>(m_pParent)->Write(stream);
	}
	if (stream)
	{
		stream->Write(GetData(), Size());
	}
}
