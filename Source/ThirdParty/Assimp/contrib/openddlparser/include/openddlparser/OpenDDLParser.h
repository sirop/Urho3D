/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2014-2015 Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#pragma once

#include <openddlparser/OpenDDLCommon.h>
#include <openddlparser/DDLNode.h>
#include <openddlparser/OpenDDLParserUtils.h>
#include <openddlparser/Value.h>

#include <vector>
#include <string>

BEGIN_ODDLPARSER_NS

class DDLNode;
class Value;

struct Identifier;
struct Reference;
struct Property;

///	@brief  Utility function to search for the next token or the end of the buffer.
/// @param  in      [in] The start position in the buffer.
/// @param  end     [in] The end position in the buffer.
///	@return Pointer showing to the next token or the end of the buffer.
///	@detail Will not increase buffer when already a valid buffer was found.
template<class T>
inline
T *lookForNextToken( T *in, T *end ) {
    while( ( isSpace( *in ) || isNewLine( *in ) || ',' == *in ) && ( in != end ) ) {
        in++;
    }
    return in;
}

///	@brief  Utility function to go for the next token or the end of the buffer.
/// @param  in      [in] The start position in the buffer.
/// @param  end     [in] The end position in the buffer.
///	@return Pointer showing to the next token or the end of the buffer.
///	@detail Will  increase buffer by a minimum of one.
template<class T>
inline
T *getNextToken( T *in, T *end ) {
    T *tmp( in );
    in = lookForNextToken( in, end );
    /*while( ( isSpace( *in ) || isNewLine( *in ) || ',' == *in ) && ( in != end ) ) {
        in++;
    }*/
    if( tmp == in ) {
        in++;
    }
    return in;
}

///	@brief  Defines the log severity.
enum LogSeverity {
    ddl_debug_msg = 0,  ///< Debug message, for debugging
    ddl_info_msg,       ///< Info messages, normal mode
    ddl_warn_msg,       ///< Parser warnings
    ddl_error_msg       ///< Parser errors
};

DLL_ODDLPARSER_EXPORT const char *getTypeToken( Value::ValueType  type );

//-------------------------------------------------------------------------------------------------
///	@class		OpenDDLParser
///	@ingroup	OpenDDLParser

///
///	@brief  This is the main API for the OpenDDL-parser.
///
/// Use instances of this class to manage the parsing and handling of your parser contexts.
//-------------------------------------------------------------------------------------------------
class DLL_ODDLPARSER_EXPORT OpenDDLParser {
public:
    ///	@brief  The log callback function pointer.
    typedef void( *logCallback )( LogSeverity severity, const std::string &msg );

public:
    ///	@brief  The default class constructor.
    OpenDDLParser();

    ///	@brief  The class constructor.
    ///	@param  buffer      [in] The buffer
    ///	@param  len         [in] Size of the buffer
    OpenDDLParser( char *buffer, size_t len );

    ///	@brief  The class destructor.
    ~OpenDDLParser();

    ///	@brief  Setter for an own log callback function.
    /// @param  callback    [in] The own callback.
    void setLogCallback( logCallback callback );

    ///	@brief  Getter for the log callback.
    /// @return The current log callback.
    logCallback getLogCallback() const;

    ///	@brief  Assigns a new buffer to parse.
    ///	@param  buffer      [in] The buffer
    ///	@param  len         [in] Size of the buffer
    void setBuffer( char *buffer, size_t len );

    ///	@brief  Assigns a new buffer to parse.
    /// @param  buffer      [in] The buffer as a std::vector.
    void setBuffer( const std::vector<char> &buffer );

    ///	@brief  Returns the buffer pointer.
    /// @return The buffer pointer.
    const char *getBuffer() const;
    
    /// @brief  Returns the size of the buffer.
    /// @return The buffer size.
    size_t getBufferSize() const;
    
    ///	@brief  Clears all parser data, including buffer and active context.
    void clear();

    ///	@brief  Starts the parsing of the OpenDDL-file.
    /// @return True in case of success, false in case of an error.
    /// @remark In case of errors check log.
    bool parse();

    bool exportContext( Context *ctx, const std::string &filename );

    ///	@brief  Returns the root node.
    /// @return The root node.
    DDLNode *getRoot() const;

    ///	@brief  Returns the parser context, only available in case of a succeeded parsing.
    /// @return Pointer to the active context or ddl_nullptr.
    Context *getContext() const;

public: // parser helpers
    char *parseNextNode( char *current, char *end );
    char *parseHeader( char *in, char *end );
    char *parseStructure( char *in, char *end );
    char *parseStructureBody( char *in, char *end, bool &error );
    void pushNode( DDLNode *node );
    DDLNode *popNode();
    DDLNode *top();
    static void normalizeBuffer( std::vector<char> &buffer );
    static char *parseName( char *in, char *end, Name **name );
    static char *parseIdentifier( char *in, char *end, Identifier **id );
    static char *parsePrimitiveDataType( char *in, char *end, Value::ValueType &type, size_t &len );
    static char *parseReference( char *in, char *end, std::vector<Name*> &names );
    static char *parseBooleanLiteral( char *in, char *end, Value **boolean );
    static char *parseIntegerLiteral( char *in, char *end, Value **integer, Value::ValueType integerType = Value::ddl_int32 );
    static char *parseFloatingLiteral( char *in, char *end, Value **floating );
    static char *parseStringLiteral( char *in, char *end, Value **stringData );
    static char *parseHexaLiteral( char *in, char *end, Value **data );
    static char *parseProperty( char *in, char *end, Property **prop );
    static char *parseDataList( char *in, char *end, Value::ValueType type, Value **data, size_t &numValues, Reference **refs, size_t &numRefs );
    static char *parseDataArrayList( char *in, char *end, DataArrayList **dataList );
    static const char *getVersion();

private:
    OpenDDLParser( const OpenDDLParser & );
    OpenDDLParser &operator = ( const OpenDDLParser & );

private:
    logCallback m_logCallback;
    std::vector<char> m_buffer;

    typedef std::vector<DDLNode*> DDLNodeStack;
    DDLNodeStack m_stack;
    Context *m_context;
};

END_ODDLPARSER_NS
