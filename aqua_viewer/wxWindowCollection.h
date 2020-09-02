#ifndef WXWINDOWCOLLECTION_H_INCLUDED
#define WXWINDOWCOLLECTION_H_INCLUDED

DECLARE_EVENT_TYPE(wxEVT_READ_CONFIG, -1)
DECLARE_EVENT_TYPE(wxEVT_WRITE_CONFIG, -1)


/**
 * Implements serialisation / deserialisation of a class of windows.
 * Parameter class is expected to implement WriteConfig() and
 * ReadConfig() to read and write config for individual windows. Ut
 * should also have a constructor that takes a parent window and a id.
 */
template <typename T>
class wxWindowCollection {
private:
	wxString make_name(std::size_t index) {
		wxString name;
		name.Printf(wxT("%s%04x"),
					collection_name_.c_str(),
					index);
		return name;
	}

	wxString collection_name_;
	wxWindow* collection_parent_;
	std::vector<T*> collection_;
	std::size_t selected_member_;

public:


	/**
	 * Constructor
	 */
	wxWindowCollection<T>(const wxString& collection_name,
						  wxWindow *parent)
		: collection_name_(collection_name),
		  collection_parent_(parent) {
		std::size_t size = readCollectionInfo();
		collection_.reserve(size + 8);
		for(std::size_t i = 0; i < size; i++)
			createCollectionMember(i);
	}

	/**
	 * "Destructor" -- writes collection back to config
	 */
	void writeCollectionInfo() {
		long tmp;
		const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
		config->SetPath(collection_name_);
		// write size
		tmp = static_cast<long>(size());
		config->Write(collection_name_, tmp);
		wxLogDebug(wxT("%s has %d members."), collection_name_.c_str(), static_cast<int>(tmp));
		// write selected member
		tmp = static_cast<long>(selected_member_);
		config->Write(wxT("selected"), tmp);
		// write individual members
		for(std::size_t index = 0; index < size(); index++) {
			config->SetPath(make_name(index));
			wxLogDebug(wxT("writing %s "), make_name(index).c_str());
			collection_[index]->WriteConfig();
			config->SetPath(wxT(".."));
		}
		config->SetPath(wxT(".."));
		aqua::Locator::checkIn(config);	

	}

	/**
	 * Reads info about the collection, returns size
	 */
	std::size_t readCollectionInfo() {
		long tmp;
		long selected;
		const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
		config->SetPath(collection_name_);
		config->Read(collection_name_, &tmp, 0L);
		wxLogDebug(wxT("%s has %d members."), collection_name_.c_str(), static_cast<int>(tmp));
		config->Read(collection_name_, &selected, 0L);
		selected_member_ = static_cast<std::size_t>(selected);
		config->SetPath(wxT(".."));
		aqua::Locator::checkIn(config);	
		return static_cast<std::size_t>(tmp);
	}

	/**
	 * Iterate through a collection with a known size
	 * and populate it.
	 */
	void createCollectionMember(std::size_t index) {
		const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
		config->SetPath(collection_name_);
		config->SetPath(make_name(index));
		T* win = new T(collection_parent_, wxNewId(), wxPoint(20,20), wxSize(320,200));
		wxLogDebug(wxT("reading %s "), make_name(index).c_str());
		win->SetName(make_name(index));
		win->ReadConfig();
		win->Restore();
		collection_.push_back(win);
		// to do -- send a read config event
		config->SetPath(wxT(".."));
		config->SetPath(wxT(".."));
		aqua::Locator::checkIn(config);	
		return;
	}

	//:
	//: Description for NewMember.
	//: @param pos <doc>
	//: @param size <doc>
	//: @return <doc>
	//:
	T* NewMember(const wxPoint& pos, const wxSize& size) {
		T* win = new T(collection_parent_, wxID_ANY, pos, size);
		win->SetName(make_name(collection_.size()));
		win->Restore();
		collection_.push_back(win);
		return win;
	}


	bool isMember(wxWindow* thing) 	{
		return (std::find(collection_.begin(), collection_.end(), thing) != collection_.end());
	}

	bool removeMember(T* thing) {
		bool result = false;
		std::vector<T*>::iterator it = std::find(collection_.begin(), collection_.end(), thing);
		result = (it != collection_.end());
		if(result) {
			collection_.erase(it);
		}
		return result;
	}

	T *get_panel(std::size_t index) {
		return collection_.at(index);
	}

	std::size_t size() 	{
		return collection_.size();
	}

	bool empty() {
		return collection_.empty();
	}

};

#endif
