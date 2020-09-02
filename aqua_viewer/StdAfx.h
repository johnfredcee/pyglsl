
//#define NOMINMAX

// order of includes is important - fbxsdk must go before wx
// or errors ensue
#pragma warning( push, 1 )
#pragma warning( disable: 4005 )
#include <fbxsdk.h>


#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/filedlg.h>
#include <wx/bitmap.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/wizard.h>
#include <wx/aui/auibook.h>
#include <wx/aui/framemanager.h>
#include <wx/log.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/buffer.h>
#include <wx/panel.h>
#include <wx/wfstream.h>
#include <wx/utils.h>
#include <wx/config.h>
#include <wx/zstream.h>
#include <wx/config.h>
#include <wx/stc/stc.h>
#include <wx/textfile.h>
#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/event.h>
#include <wx/clntdata.h>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <PbSystemTypes.h>
#include <Locator.h>
#include <CRC/CRC.h>
#include <Utils/Array.h>
#include <Utils/LoggerOutputFunctor.h>
#include <Utils/Logger.h>
#include <Utils/HashString.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>
#include <MetaData/Dynamic/MetaData_WA.h>

#pragma warning( error: 4005 )
#pragma warning( pop )

#define PBUNREFERENCED_PARAMETER(x) (void)(x)



