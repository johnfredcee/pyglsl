#ifndef METABITMAPMANAGER_H
#define METABITMAPMANAGER_H

class MetaBitmapManager : public aqua::Locator::ServiceSingleton<MetaBitmapManager>
{
	friend class aqua::Locator::ServiceSingleton<MetaBitmapManager>;
public:
    MetaBitmapManager();

	virtual ~MetaBitmapManager();

	crc32 addBitmap(const std::string &name, const BitmapData& tex);

	void getBitmapName(crc32& bitmapName, std::string& name);

private:
	std::map<crc32, std::string> bitmapNames_;
};

#endif
