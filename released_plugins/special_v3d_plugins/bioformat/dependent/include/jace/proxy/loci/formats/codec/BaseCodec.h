#ifndef JACE_PROXY_LOCI_FORMATS_CODEC_BASECODEC_H
#define JACE_PROXY_LOCI_FORMATS_CODEC_BASECODEC_H

#ifndef JACE_OS_DEP_H
#include "jace/os_dep.h"
#endif

#ifndef JACE_NAMESPACE_H
#include "jace/namespace.h"
#endif

#ifndef JACE_JOBJECT_H
#include "jace/proxy/JObject.h"
#endif

#ifndef JACE_JARRAY_H
#include "jace/JArray.h"
#endif

#ifndef JACE_JFIELD_PROXY_H
#include "jace/JFieldProxy.h"
#endif

#ifndef JACE_JCLASSIMPL_H
#include "jace/JClassImpl.h"
#endif

/**
 * The super class for this class.
 *
 */
#ifndef JACE_PROXY_JAVA_LANG_OBJECT_H
#include "jace/proxy/java/lang/Object.h"
#endif

/**
 * The interfaces implemented by this class.
 *
 */
#ifndef JACE_PROXY_LOCI_FORMATS_CODEC_CODEC_H
#include "jace/proxy/loci/formats/codec/Codec.h"
#endif

/**
 * Classes which this class is fully dependent upon.
 *
 */
#ifndef JACE_TYPES_JBYTE_H
#include "jace/proxy/types/JByte.h"
#endif

/**
 * Forward declarations for the classes that this class uses.
 *
 */
BEGIN_NAMESPACE_5( jace, proxy, loci, formats, codec )
class CodecOptions;
END_NAMESPACE_5( jace, proxy, loci, formats, codec )

BEGIN_NAMESPACE_4( jace, proxy, java, io )
class IOException;
END_NAMESPACE_4( jace, proxy, java, io )

BEGIN_NAMESPACE_4( jace, proxy, loci, common )
class RandomAccessInputStream;
END_NAMESPACE_4( jace, proxy, loci, common )

BEGIN_NAMESPACE_4( jace, proxy, loci, formats )
class FormatException;
END_NAMESPACE_4( jace, proxy, loci, formats )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JVoid;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, codec )

/**
 * The Jace C++ proxy class for loci.formats.codec.BaseCodec.
 * Please do not edit this class, as any changes you make will be overwritten.
 * For more information, please refer to the Jace Developer's Guide.
 *
 */
class BaseCodec : public virtual ::jace::proxy::java::lang::Object, public virtual ::jace::proxy::loci::formats::codec::Codec
{
public: 

/**
 * BaseCodec
 *
 */
BaseCodec();

/**
 * test
 *
 */
void test();

/**
 * compress
 *
 */
::jace::JArray< ::jace::proxy::types::JByte > compress( ::jace::JArray< ::jace::JArray< ::jace::proxy::types::JByte > > p0, ::jace::proxy::loci::formats::codec::CodecOptions p1 );

/**
 * decompress
 *
 */
::jace::JArray< ::jace::proxy::types::JByte > decompress( ::jace::JArray< ::jace::proxy::types::JByte > p0 );

/**
 * decompress
 *
 */
::jace::JArray< ::jace::proxy::types::JByte > decompress( ::jace::JArray< ::jace::JArray< ::jace::proxy::types::JByte > > p0 );

/**
 * decompress
 *
 */
::jace::JArray< ::jace::proxy::types::JByte > decompress( ::jace::JArray< ::jace::proxy::types::JByte > p0, ::jace::proxy::loci::formats::codec::CodecOptions p1 );

/**
 * decompress
 *
 */
::jace::JArray< ::jace::proxy::types::JByte > decompress( ::jace::proxy::loci::common::RandomAccessInputStream p0, ::jace::proxy::loci::formats::codec::CodecOptions p1 );

/**
 * decompress
 *
 */
::jace::JArray< ::jace::proxy::types::JByte > decompress( ::jace::JArray< ::jace::JArray< ::jace::proxy::types::JByte > > p0, ::jace::proxy::loci::formats::codec::CodecOptions p1 );

/**
 * The following methods are required to integrate this class
 * with the Jace framework.
 *
 */
BaseCodec( jvalue value );
BaseCodec( jobject object );
BaseCodec( const BaseCodec& object );
BaseCodec( const NoOp& noOp );
virtual const JClass& getJavaJniClass() const throw ( ::jace::JNIException );
static const JClass& staticGetJavaJniClass() throw ( ::jace::JNIException );
};

END_NAMESPACE_5( jace, proxy, loci, formats, codec )

BEGIN_NAMESPACE( jace )

#ifndef PUT_TSDS_IN_HEADER
  template <> ElementProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::ElementProxy( jarray array, jvalue element, int index );
  template <> ElementProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::codec::BaseCodec>& proxy );
#else
  template <> inline ElementProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::ElementProxy( jarray array, jvalue element, int index ) : 
    ::jace::proxy::loci::formats::codec::BaseCodec( element ), Object( NO_OP ), mIndex( index )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, array ) );
  }
  template <> inline ElementProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::codec::BaseCodec>& proxy ) : 
    ::jace::proxy::loci::formats::codec::BaseCodec( proxy.getJavaJniObject() ), Object( NO_OP ), mIndex( proxy.mIndex )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, proxy.parent ) );
  }
#endif

#ifndef PUT_TSDS_IN_HEADER
  template <> JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>& object );
#else
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ ) : 
    ::jace::proxy::loci::formats::codec::BaseCodec( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    if ( parent_ )
      parent = ::jace::helper::newGlobalRef( env, parent_ );
    else
      parent = parent_;

    parentClass = 0;
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ ) : 
    ::jace::proxy::loci::formats::codec::BaseCodec( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    parent = 0;
    parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, parentClass_ ) );
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::codec::BaseCodec>& object ) : 
    ::jace::proxy::loci::formats::codec::BaseCodec( object.getJavaJniValue() ), Object( NO_OP )
  {
    fieldID = object.fieldID; 

    if ( object.parent )
    {
      JNIEnv* env = ::jace::helper::attach();
      parent = ::jace::helper::newGlobalRef( env, object.parent );
    }
    else
      parent = 0;

    if ( object.parentClass )
    {
      JNIEnv* env = ::jace::helper::attach();
      parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, object.parentClass ) );
    }
    else
      parentClass = 0;
  }
#endif

END_NAMESPACE( jace )

#endif // #ifndef JACE_PROXY_LOCI_FORMATS_CODEC_BASECODEC_H

