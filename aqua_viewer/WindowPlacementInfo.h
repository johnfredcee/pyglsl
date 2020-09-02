#if !defined(WINDOWPLACEMENTINFO_H_INCLUDED)
#define WINDOWPLACEMENTINFO_H_INCLUDED


struct WindowPlacementInfo
{
 
	long windowX_;
	long windowY_;
	long windowW_;
	long windowH_;
 
	void Read()
	{
		const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
		config->Read(wxT("WindowX"), &windowX_, 20);
		config->Read(wxT("WindowY"), &windowY_, 20);
 
		config->Read(wxT("WindowW"), &windowW_, 640);
		config->Read(wxT("WindowH"), &windowH_, 480);
		aqua::Locator::checkIn(config);	
	}
 
	void Write()
	{
		const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
		config->Write(wxT("WindowX"), windowX_);
		config->Write(wxT("WindowY"), windowY_);
 
		config->Write(wxT("WindowW"), windowW_);
		config->Write(wxT("WindowH"), windowH_);
		aqua::Locator::checkIn(config);	
	}
};


#endif
