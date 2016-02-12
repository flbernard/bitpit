/*---------------------------------------------------------------------------*\
 *
 *  bitpit
 *
 *  Copyright (C) 2015-2016 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of bitbit.
 *
 *  bitpit is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  bitpit is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with bitpit. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/

#include "VTK.hpp"

namespace bitpit{

/*!
 *  Returns number of vertices for a given element type.
 *  Codification of element type according to http://www.vtk.org/wp-content/uploads/2015/04/file-formats.pdf
 *  @param[in]  t           element type
 *  \return     number of vertices of element type
 */
uint8_t vtk::getNNodeInElement( const VTKElementType & t){
    int myType = static_cast<std::underlying_type<VTKElementType>::type>(t);

    switch (myType){

        case 1: 
            return(1);
        case 3: 
            return(2);
        case 5: case 21: 
            return(3);
        case 8: case 9:  case 10: 
            return(4);
        case 11: case 12: case 23: case 24: 
            return(8);
        case 13: case 22: 
            return(6);
        case 14: 
            return(5) ;
        case 25: 
            return(20);
        default:
           return(-1);
    };


};

/*!
 * Converts a string into a Field information.
 * @param[in]   line_       string to be converted
 * @param[out]  data_       Field information
 * \return      true if successful
 */
bool vtk::convertStringToDataArray( const std::string &line_, VTKField &data_  ){

    std::string type_, name_, code_, comp_, offs_ ;
    int         components_(1), offset_ ;

    VTKFormat    codex ;
    VTKDataType  type ;
    VTKFieldType comp(VTKFieldType::SCALAR) ;

    bool  success(true) ;


    if( bitpit::utils::keywordInString( line_, "<DataArray ") ){  
        success = success && bitpit::utils::getAfterKeyword( line_, "type=", '\"', type_) ;
        success = success && bitpit::utils::getAfterKeyword( line_, "Name=", '\"', name_) ;
        success = success && bitpit::utils::getAfterKeyword( line_, "format=", '\"', code_) ;

        if( bitpit::utils::getAfterKeyword( line_, "NumberOfComponents=", '\"', comp_)  ){
            bitpit::utils::convertString( comp_, components_ ) ;
        };

        if(components_==3)
            comp=VTKFieldType::VECTOR ;

        vtk::convertStringToEnum( type_, type) ;
        vtk::convertStringToEnum( code_, codex) ;

        data_.setDataType(type) ;
        data_.setName(name_) ;
        data_.setCodification(codex) ;
        if(name_ != "connectivity") 
            data_.setFieldType(comp) ;

        if(code_=="appended") {
            if( bitpit::utils::getAfterKeyword( line_, "offset=", '\"', offs_) ){
                bitpit::utils::convertString( offs_, offset_ ) ;
                data_.setOffset(offset_) ;
            }
            else{
                success = false ;
            };
        }

        return success ;
    }

    else{
        return false ;
    };


};

/*!
 * Converts a Field information to string as requested by VTK format.
 * @param[in]  field_       Field information
 * @return string in VTK format
 */
std::string  vtk::convertDataArrayToString( const VTKField &field_ ){

    std::stringstream   os("") ;
    unsigned            comp = static_cast<unsigned>(field_.getFieldType())  ;

    if( field_.getFieldType() != VTKFieldType::SCALAR && field_.getFieldType() != VTKFieldType::VECTOR )
        comp = 1 ;

    os << "        <DataArray "
        << "type=\"" << vtk::convertEnumToString( field_.getDataType() ) << "\" "
        << "Name=\"" << field_.getName() << "\" "
        << "NumberOfComponents=\""<< comp << "\" "
        << "format=\"" << vtk::convertEnumToString(field_.getCodification()) << "\" ";

    if( field_.getCodification() == VTKFormat::APPENDED ){
        os << "offset=\"" << field_.getOffset() << "\" " ;
    };

    os << ">" ;

    return( os.str() ) ;       


};

/*!
 * Converts a parallel field information to string as requested by VTK format.
 * @param[in]  field_       Field information
 * @return string in VTK format
 */
std::string  vtk::convertPDataArrayToString( const VTKField &field_ ){

    std::stringstream  os("") ;

    os << "        <PDataArray "
        << "type=\"" << vtk::convertEnumToString(field_.getDataType()) << "\" "
        << "Name=\"" << field_.getName() << "\" "
        << "NumberOfComponents=\""<< unsigned(field_.getComponents()) << "\" " 
        << ">" ;

    return( os.str() ) ;

};

/*!
 * Converts a VTKLocation into string for DataArray format
 * @param[in]  loc VTKLocation to be converted
 * @return string to be used in DataArray
 */
std::string vtk::convertEnumToString( const VTKLocation &loc ){
   
    switch(loc){
        case VTKLocation::CELL :
            return("Cell");
        case VTKLocation::POINT :
            return("Point");
        case VTKLocation::UNDEFINED :
            return("Undefined") ;
        default:
            return("Undefined") ;
    };
};

/*!
 * Converts a VTKFormat into string for DataArray format
 * @param[in]  cod VTKFormat to be converted
 * @return string to be used in DataArray
 */
std::string vtk::convertEnumToString( const VTKFormat &cod ){
    
    switch(cod){
        case VTKFormat::ASCII :
            return("ascii");
        case VTKFormat::APPENDED :
            return("appended");
        case VTKFormat::UNDEFINED :
            return("Undefined") ;
        default:
            return("Undefined") ;
    };
};

/*!
 * Converts a VTKDataType into string for DataArray format
 * @param[in]  type VTKDataType to be converted
 * @return string to be used in DataArray
 */
std::string vtk::convertEnumToString( const VTKDataType &type ){
    
    switch(type){
        case VTKDataType::Int8 :
            return("Int8");
        case VTKDataType::Int16 :
            return("Int16");
        case VTKDataType::Int32 :
            return("Int32");
        case VTKDataType::Int64 :
            return("Int64");
        case VTKDataType::UInt8 :
            return("UInt8");
        case VTKDataType::UInt16 :
            return("UInt16");
        case VTKDataType::UInt32 :
            return("UInt32");
        case VTKDataType::UInt64 :
            return("UInt64");
        case VTKDataType::Float32 :
            return("Float32");
        case VTKDataType::Float64 :
            return("Float64");
        case VTKDataType::UNDEFINED :
            return("Undefined");
        default:
            return("Undefined") ;
    };
};

/*!
 * Converts a std::string as read in vtk file to VTKLocation
 * @param[in] str string read in DataArray header
 * @param[out]  loc VTKLocation
 * @return  if str contained expected value
 */
bool vtk::convertStringToEnum(  const std::string &str, VTKLocation &loc ){
    

    if( str == "Cell"){
        loc = VTKLocation::CELL;
        return(true);

    } else if ( str == "Point" ){
        loc = VTKLocation::POINT;
        return(true);

    } else {
        loc = VTKLocation::UNDEFINED ;
        return(false);
    };

};

/*!
 * Converts a std::string as read in vtk file to VTKFormat  
 * @param[in] str string read in DataArray header
 * @param[out]  cod VTKFormat
 * @return  if str contained expected value
 */
bool vtk::convertStringToEnum( const  std::string &str, VTKFormat &cod ){
    

    if( str == "ascii"){
        cod = VTKFormat::ASCII ;
        return(true);

    } else if ( str == "appended" ){
        cod = VTKFormat::APPENDED ;
        return(true);

    } else {
        cod = VTKFormat::UNDEFINED ;
        return(false);
    };

};

/*!
 * Converts a std::string as read in vtk file to VTKDataType 
 * @param[in] str string read in DataArray header
 * @param[out]  type VTKDataType to be converted
 * @return  if str contained expected value
 */
bool vtk::convertStringToEnum( const std::string &str, VTKDataType &type ){

    if ( str =="Int8" ){
        type = VTKDataType::Int8;
        return(true);
    }

    else if ( str =="Int16" ){
        type = VTKDataType::Int16;
        return(true);
    }

    else if ( str =="Int32" ){
        type = VTKDataType::Int32;
        return(true);
    }

    else if ( str =="Int64" ){
        type = VTKDataType::Int64;
        return(true);
    }

    else if ( str =="UInt8" ){
        type = VTKDataType::UInt8;
        return(true);
    }

    else if ( str =="UInt16" ){
        type = VTKDataType::UInt16;
        return(true);
    }

    else if ( str =="UInt32" ){
        type = VTKDataType::UInt32;
        return(true);
    }

    else if ( str =="UInt64" ){
        type = VTKDataType::UInt64;
        return(true);
    }

    else if ( str =="Float32" ){
        type = VTKDataType::Float32;
        return(true);
    }

    else if ( str =="Float64" ){
        type = VTKDataType::Float64;
        return(true);
    }

    else {
        type = VTKDataType::UNDEFINED;
        return(false);
    }

};


}
