#ifndef METADATAUTILS_H
#define METADATAUTILS_H

class DynamicMetaData;

/** Look for the meta_header directory by scanning upward */
void findMetaHeaderDirectory();

/** Grab the latest meta_headers directory from svn */
void updateMetaDataHeaders();

/**
 * Read a subdirectory full of metadata headers into the given meta data object
 * @param md MetaDataObject
 * @param subDirectory Directory to read headers in
 */
void readMetaDataHeaders(aqua::shared_ptr<DynamicMetaData> md, const wxString subDirectory);

#endif
