#ifndef JACE_PROXY_LOCI_FORMATS_TIFF_PHOTOINTERP_H
#define JACE_PROXY_LOCI_FORMATS_TIFF_PHOTOINTERP_H

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
 * Classes which this class is fully dependent upon.
 *
 */
#ifndef JACE_TYPES_JINT_H
#include "jace/proxy/types/JInt.h"
#endif

/**
 * Forward declarations for the classes that this class uses.
 *
 */
BEGIN_NAMESPACE_4( jace, proxy, java, lang )
class String;
END_NAMESPACE_4( jace, proxy, java, lang )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JInt;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_4( jace, proxy, loci, formats )
class FormatException;
END_NAMESPACE_4( jace, proxy, loci, formats )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JVoid;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, tiff )

/**
 * The Jace C++ proxy class for loci.formats.tiff.PhotoInterp.
 * Please do not edit this class, as any changes you make will be overwritten.
 * For more information, please refer to the Jace Developer's Guide.
 *
 */
class PhotoInterp : public virtual ::jace::proxy::java::lang::Object
{
public: 

/**
 * getPIName
 *
 */
static ::jace::proxy::java::lang::String getPIName( ::jace::proxy::types::JInt p0 );

/**
 * getPIMeta
 *
 */
static ::jace::proxy::java::lang::String getPIMeta( ::jace::proxy::types::JInt p0 );

/**
 * checkPI
 *
 */
static void checkPI( ::jace::proxy::types::JInt p0 );

/**
 * public static final WHITE_IS_ZERO
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > WHITE_IS_ZERO();

/**
 * public static final BLACK_IS_ZERO
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > BLACK_IS_ZERO();

/**
 * public static final RGB
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > JACE_RGB();

/**
 * public static final RGB_PALETTE
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > RGB_PALETTE();

/**
 * public static final TRANSPARENCY_MASK
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > TRANSPARENCY_MASK();

/**
 * public static final CMYK
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > JACE_CMYK();

/**
 * public static final Y_CB_CR
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > Y_CB_CR();

/**
 * public static final CIE_LAB
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > CIE_LAB();

/**
 * public static final CFA_ARRAY
 *
 */
static ::jace::JFieldProxy< ::jace::proxy::types::JInt > CFA_ARRAY();

/**
 * The following methods are required to integrate this class
 * with the Jace framework.
 *
 */
PhotoInterp( jvalue value );
PhotoInterp( jobject object );
PhotoInterp( const PhotoInterp& object );
PhotoInterp( const NoOp& noOp );
virtual const JClass& getJavaJniClass() const throw ( ::jace::JNIException );
static const JClass& staticGetJavaJniClass() throw ( ::jace::JNIException );
};

END_NAMESPACE_5( jace, proxy, loci, formats, tiff )

BEGIN_NAMESPACE( jace )

#ifndef PUT_TSDS_IN_HEADER
  template <> ElementProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::ElementProxy( jarray array, jvalue element, int index );
  template <> ElementProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>& proxy );
#else
  template <> inline ElementProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::ElementProxy( jarray array, jvalue element, int index ) : 
    ::jace::proxy::loci::formats::tiff::PhotoInterp( element ), Object( NO_OP ), mIndex( index )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, array ) );
  }
  template <> inline ElementProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>& proxy ) : 
    ::jace::proxy::loci::formats::tiff::PhotoInterp( proxy.getJavaJniObject() ), Object( NO_OP ), mIndex( proxy.mIndex )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, proxy.parent ) );
  }
#endif

#ifndef PUT_TSDS_IN_HEADER
  template <> JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>& object );
#else
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ ) : 
    ::jace::proxy::loci::formats::tiff::PhotoInterp( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    if ( parent_ )
      parent = ::jace::helper::newGlobalRef( env, parent_ );
    else
      parent = parent_;

    parentClass = 0;
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ ) : 
    ::jace::proxy::loci::formats::tiff::PhotoInterp( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    parent = 0;
    parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, parentClass_ ) );
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::tiff::PhotoInterp>& object ) : 
    ::jace::proxy::loci::formats::tiff::PhotoInterp( object.getJavaJniValue() ), Object( NO_OP )
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

#endif // #ifndef JACE_PROXY_LOCI_FORMATS_TIFF_PHOTOINTERP_H

