#ifndef METASERIALIZER_H
#define METASERIALIZER_H

template <typename T> void MetaSerialiser(const T& data, DynamicMetaParser& parser);

template <typename T> void MetaDeserialiser(T& data, DynamicMetaParser& parser);

#endif
