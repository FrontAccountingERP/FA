<?php
/*
*  Module written/ported by Xavier Noguer <xnoguer@rezebra.com>
*
*  The majority of this is _NOT_ my code.  I simply ported it from the
*  PERL Spreadsheet::WriteExcel module.
*
*  The author of the Spreadsheet::WriteExcel module is John McNamara 
*  <jmcnamara@cpan.org>
*
*  I _DO_ maintain this code, and John McNamara has nothing to do with the
*  porting of this code to PHP.  Any questions directly related to this
*  class library should be directed to me.
*
*  License Information:
*
*    Spreadsheet::WriteExcel:  A library for generating Excel Spreadsheets
*    Copyright (C) 2002 Xavier Noguer xnoguer@rezebra.com
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Lesser General Public
*    License as published by the Free Software Foundation; either
*    version 2.1 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public
*    License along with this library; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
* @const ADD token identifier for character "+"
*/
define('ADD',"+");

/**
* @const SUB token identifier for character "-"
*/
define('SUB',"-");

/**
* @const EQUAL token identifier for character "="
*/
define('EQUAL',"=");

/**
* @const MUL token identifier for character "*"
*/
define('MUL',"*");

/**
* @const DIV token identifier for character "/"
*/
define('DIV',"/");

/**
* @const OPEN token identifier for character "("
*/
define('OPEN',"(");

/**
* @const CLOSE token identifier for character ")"
*/
define('CLOSE',")");

/**
* @const COMA token identifier for character ","
*/
define('COMA',",");

/**
* Class for writing Excel BIFF records.
* 
* From "MICROSOFT EXCEL BINARY FILE FORMAT" by Mark O'Brien (Microsoft Corporation):
*
* BIFF (BInary File Format) is the file format in which Excel documents are 
* saved on disk.  A BIFF file is a complete description of an Excel document.
* BIFF files consist of sequences of variable-length records. There are many 
* different types of BIFF records.  For example, one record type describes a 
* formula entered into a cell; one describes the size and location of a 
* window into a document; another describes a picture format.
*
* @author Xavier Noguer <xnoguer@rezebra.com>
* @package Spreadsheet_WriteExcel
*/

class BIFFWriter
{
    var $_BIFF_version = 0x0500;

/**
* Constructor
*
* @access public
*/
    function BIFFwriter()
    {
        // The byte order of this architecture. 0 => little endian, 1 => big endian
        $this->_byte_order = '';
        // The string containing the data of the BIFF stream
        $this->_data       = '';
        // Should be the same as strlen($this->_data)
        $this->_datasize   = 0;
        // The maximun length for a BIFF record. See _add_continue()
        $this->_limit      = 2080;   
        // Set the byte order
        $this->_set_byte_order();
    }

/**
* Determine the byte order and store it as class data to avoid
* recalculating it for each call to new().
*
* @access private
*/
    function _set_byte_order()
    {
        if ($this->_byte_order == '')
        {
            // Check if "pack" gives the required IEEE 64bit float
            $teststr = pack("d", 1.2345);
            $number  = pack("C8", 0x8D, 0x97, 0x6E, 0x12, 0x83, 0xC0, 0xF3, 0x3F);
            if ($number == $teststr) {
                $byte_order = 0;    // Little Endian
            }
            elseif ($number == strrev($teststr)){
                $byte_order = 1;    // Big Endian
            }
            else {
                // Give up. I'll fix this in a later version.
                die("Required floating point format not supported ".
                    "on this platform. See the portability section ".
                    "of the documentation."
                   );
            }
        }
        $this->_byte_order = $byte_order;
    }

/**
* General storage function
*
* @param string $data binary data to prepend
* @access private
*/
    function _prepend($data)
    {
        if (strlen($data) > $this->_limit) {
            $data = $this->_add_continue($data);
        }
        $this->_data      = $data.$this->_data;
        $this->_datasize += strlen($data);
    }

/**
* General storage function
*
* @param string $data binary data to append
* @access private
*/
    function _append($data)
    {
        if (strlen($data) > $this->_limit) {
            $data = $this->_add_continue($data);
        }
        $this->_data      = $this->_data.$data;
        $this->_datasize += strlen($data);
    }

/**
* Writes Excel BOF record to indicate the beginning of a stream or
* sub-stream in the BIFF file.
*
* @param  integer $type type of BIFF file to write: 0x0005 Workbook, 0x0010 Worksheet.
* @access private
*/
    function _store_bof($type)
    {
        $record  = 0x0809;        // Record identifier
        $length  = 0x0008;        // Number of bytes to follow
        $version = $this->_BIFF_version;
   
        // According to the SDK $build and $year should be set to zero.
        // However, this throws a warning in Excel 5. So, use these
        // magic numbers.
        $build   = 0x096C;
        $year    = 0x07C9;
   
        $header  = pack("vv",   $record, $length);
        $data    = pack("vvvv", $version, $type, $build, $year);
        $this->_prepend($header.$data);
    }

/**
* Writes Excel EOF record to indicate the end of a BIFF stream.
*
* @access private
*/
    function _store_eof() 
    {
        $record    = 0x000A;   // Record identifier
        $length    = 0x0000;   // Number of bytes to follow
        $header    = pack("vv", $record, $length);
        $this->_append($header);
    }

/**
* Excel limits the size of BIFF records. In Excel 5 the limit is 2084 bytes. In
* Excel 97 the limit is 8228 bytes. Records that are longer than these limits
* must be split up into CONTINUE blocks.
*
* This function takes a long BIFF record and inserts CONTINUE records as
* necessary.
*
* @param  string  $data The original binary data to be written
* @return string        A very convenient string of continue blocks
* @access private
*/
    function _add_continue($data)
    {
        $limit      = $this->_limit;
        $record     = 0x003C;         // Record identifier
 
        // The first 2080/8224 bytes remain intact. However, we have to change
        // the length field of the record.
        $tmp = substr($data, 0, 2).pack("v", $limit-4).substr($data, 4, $limit - 4);
        
        $header = pack("vv", $record, $limit);  // Headers for continue records
 
        // Retrieve chunks of 2080/8224 bytes +4 for the header.
        for($i = $limit; $i < strlen($data) - $limit; $i += $limit)
        {
            $tmp .= $header;
            $tmp .= substr($data, $i, $limit);
        }

        // Retrieve the last chunk of data
        $header  = pack("vv", $record, strlen($data) - $i);
        $tmp    .= $header;
        $tmp    .= substr($data,$i,strlen($data) - $i);
 
        return($tmp);
    }
}

/**
* Class for creating OLE streams for Excel Spreadsheets
*
* @author Xavier Noguer <xnoguer@rezebra.com>
* @package Spreadsheet_WriteExcel
*/
class OLEwriter
{
    /**
    * Filename for the OLE stream
    * @var string
    * @see _initialize()
    */
    var $_OLEfilename;

    /**
    * Filehandle for the OLE stream
    * @var resource
    */
    var $_filehandle;

    /**
    * Name of the temporal file in case OLE stream goes to stdout
    * @var string
    */
    var $_tmp_filename;

    /**
    * Variable for preventing closing two times
    * @var integer
    */
    var $_fileclosed;

    /**
    * Size of the data to be written to the OLE stream
    * @var integer
    */
    var $_biffsize;

    /**
    * Real data size to be written to the OLE stream
    * @var integer
    */
    var $_booksize;

    /**
    * Number of big blocks in the OLE stream
    * @var integer
    */
    var $_big_blocks;

    /**
    * Number of list blocks in the OLE stream
    * @var integer
    */
    var $_list_blocks;

    /**
    * Number of big blocks in the OLE stream
    * @var integer
    */
    var $_root_start;

    /**
    * Class for creating an OLEwriter
    *
    * @param string $OLEfilename the name of the file for the OLE stream
    */
    function OLEwriter($OLEfilename)
    {
        $this->_OLEfilename  = $OLEfilename;
        $this->_filehandle   = "";
        $this->_tmp_filename = "";
        $this->_fileclosed   = 0;
        //$this->_size_allowed = 0;
        $this->_biffsize     = 0;
        $this->_booksize     = 0;
        $this->_big_blocks   = 0;
        $this->_list_blocks  = 0;
        $this->_root_start   = 0;
        //$this->_block_count  = 4;
        $this->_initialize();
    }

/**
* Check for a valid filename and store the filehandle.
* Filehandle "-" writes to STDOUT
*/
    function _initialize()
    {
        $OLEfile = $this->_OLEfilename;
 
        if(($OLEfile == '-') or ($OLEfile == ''))
        {
            $this->_tmp_filename = tempnam("/tmp", "OLEwriter");
            $fh = fopen($this->_tmp_filename,"wb");
            if ($fh == false) {
                die("Can't create temporary file.");
            }
        }
        else
        {
            // Create a new file, open for writing (in binmode)
            $fh = fopen($OLEfile,"wb");
            if ($fh == false) {
                die("Can't open $OLEfile. It may be in use or protected.");
            }
        }

        // Store filehandle
        $this->_filehandle = $fh;
    }


    /**
    * Set the size of the data to be written to the OLE stream.
    * The maximun size comes from this:
    *   $big_blocks = (109 depot block x (128 -1 marker word)
    *                 - (1 x end words)) = 13842
    *   $maxsize    = $big_blocks * 512 bytes = 7087104
    *
    * @access public
    * @see Workbook::store_OLE_file()
    * @param integer $biffsize The size of the data to be written to the OLE stream
    * @return integer 1 for success
    */
    function set_size($biffsize)
    {
        $maxsize = 7087104; // TODO: extend max size
 
        if ($biffsize > $maxsize) {
            die("Maximum file size, $maxsize, exceeded.");
        }
 
        $this->_biffsize = $biffsize;
        // Set the min file size to 4k to avoid having to use small blocks
        if ($biffsize > 4096) {
            $this->_booksize = $biffsize;
        }
        else {
            $this->_booksize = 4096;
        }
        //$this->_size_allowed = 1;
        return(1);
    }


    /**
    * Calculate various sizes needed for the OLE stream
    */
    function _calculate_sizes()
    {
        $datasize = $this->_booksize;
        if ($datasize % 512 == 0) {
            $this->_big_blocks = $datasize/512;
        }
        else {
            $this->_big_blocks = floor($datasize/512) + 1;
        }
        // There are 127 list blocks and 1 marker blocks for each big block
        // depot + 1 end of chain block
        $this->_list_blocks = floor(($this->_big_blocks)/127) + 1;
        $this->_root_start  = $this->_big_blocks;
    }

    /**
    * Write root entry, big block list and close the filehandle.
    * This routine is used to explicitly close the open filehandle without
    * having to wait for DESTROY.
    *
    * @access public
    * @see Workbook::store_OLE_file()
    */
    function close() 
    {
        //return if not $this->{_size_allowed};
        $this->_write_padding();
        $this->_write_property_storage();
        $this->_write_big_block_depot();
        // Close the filehandle 
        fclose($this->_filehandle);
        if(($this->_OLEfilename == '-') or ($this->_OLEfilename == ''))
        {
            $fh = fopen($this->_tmp_filename, "rb");
            if ($fh == false) {
                die("Can't read temporary file.");
            }
            fpassthru($fh);
            // Delete the temporary file.
            @unlink($this->_tmp_filename);
        }
        $this->_fileclosed = 1;
    }


    /**
    * Write BIFF data to OLE file.
    *
    * @param string $data string of bytes to be written
    */
    function write($data) //por ahora sólo a STDOUT
    {
        fwrite($this->_filehandle,$data,strlen($data));
    }


    /**
    * Write OLE header block.
    */
    function write_header()
    {
        $this->_calculate_sizes();
        $root_start      = $this->_root_start;
        $num_lists       = $this->_list_blocks;
        $id              = pack("nnnn", 0xD0CF, 0x11E0, 0xA1B1, 0x1AE1);
        $unknown1        = pack("VVVV", 0x00, 0x00, 0x00, 0x00);
        $unknown2        = pack("vv",   0x3E, 0x03);
        $unknown3        = pack("v",    -2);
        $unknown4        = pack("v",    0x09);
        $unknown5        = pack("VVV",  0x06, 0x00, 0x00);
        $num_bbd_blocks  = pack("V",    $num_lists);
        $root_startblock = pack("V",    $root_start);
        $unknown6        = pack("VV",   0x00, 0x1000);
        $sbd_startblock  = pack("V",    -2);
        $unknown7        = pack("VVV",  0x00, -2 ,0x00);
        $unused          = pack("V",    -1);
 
        fwrite($this->_filehandle,$id);
        fwrite($this->_filehandle,$unknown1);
        fwrite($this->_filehandle,$unknown2);
        fwrite($this->_filehandle,$unknown3);
        fwrite($this->_filehandle,$unknown4);
        fwrite($this->_filehandle,$unknown5);
        fwrite($this->_filehandle,$num_bbd_blocks);
        fwrite($this->_filehandle,$root_startblock);
        fwrite($this->_filehandle,$unknown6);
        fwrite($this->_filehandle,$sbd_startblock);
        fwrite($this->_filehandle,$unknown7);
 
        for($i=1; $i <= $num_lists; $i++)
        {
            $root_start++;
            fwrite($this->_filehandle,pack("V",$root_start));
        }
        for($i = $num_lists; $i <=108; $i++)
        {
            fwrite($this->_filehandle,$unused);
        }
    }


    /**
    * Write big block depot.
    */
    function _write_big_block_depot()
    {
        $num_blocks   = $this->_big_blocks;
        $num_lists    = $this->_list_blocks;
        $total_blocks = $num_lists *128;
        $used_blocks  = $num_blocks + $num_lists +2;
 
        $marker       = pack("V", -3);
        $end_of_chain = pack("V", -2);
        $unused       = pack("V", -1);
 
        for($i=1; $i < $num_blocks; $i++)
        {
            fwrite($this->_filehandle,pack("V",$i));
        }
        fwrite($this->_filehandle,$end_of_chain);
        fwrite($this->_filehandle,$end_of_chain);
        for($i=0; $i < $num_lists; $i++)
        {
            fwrite($this->_filehandle,$marker);
        }
        for($i=$used_blocks; $i <= $total_blocks; $i++)
        {
            fwrite($this->_filehandle,$unused);
        }
    }

/**
* Write property storage. TODO: add summary sheets
*/
    function _write_property_storage()
    {
        //$rootsize = -2;
        /***************  name         type   dir start size */
        $this->_write_pps("Root Entry", 0x05,   1,   -2, 0x00);
        $this->_write_pps("Book",       0x02,  -1, 0x00, $this->_booksize);
        $this->_write_pps('',           0x00,  -1, 0x00, 0x0000);
        $this->_write_pps('',           0x00,  -1, 0x00, 0x0000);
    }

/**
* Write property sheet in property storage
*
* @param string  $name  name of the property storage.
* @param integer $type  type of the property storage.
* @param integer $dir   dir of the property storage.
* @param integer $start start of the property storage.
* @param integer $size  size of the property storage.
* @access private
*/
    function _write_pps($name,$type,$dir,$start,$size)
    {
        $length  = 0;
        $rawname = '';
 
        if ($name != '')
        {
            $name = $name . "\0";
            for($i=0;$i<strlen($name);$i++)
            {
                // Simulate a Unicode string
                $rawname .= pack("H*",dechex(ord($name{$i}))).pack("C",0);
            }
            $length = strlen($name) * 2;
        }
       
        $zero            = pack("C",  0);
        $pps_sizeofname  = pack("v",  $length);    // 0x40
        $pps_type        = pack("v",  $type);      // 0x42
        $pps_prev        = pack("V",  -1);         // 0x44
        $pps_next        = pack("V",  -1);         // 0x48
        $pps_dir         = pack("V",  $dir);       // 0x4c
       
        $unknown1        = pack("V",  0);
       
        $pps_ts1s        = pack("V",  0);          // 0x64
        $pps_ts1d        = pack("V",  0);          // 0x68
        $pps_ts2s        = pack("V",  0);          // 0x6c
        $pps_ts2d        = pack("V",  0);          // 0x70
        $pps_sb          = pack("V",  $start);     // 0x74
        $pps_size        = pack("V",  $size);      // 0x78
       
       
        fwrite($this->_filehandle,$rawname);
        for($i=0; $i < (64 -$length); $i++) {
            fwrite($this->_filehandle,$zero);
        }
        fwrite($this->_filehandle,$pps_sizeofname);
        fwrite($this->_filehandle,$pps_type);
        fwrite($this->_filehandle,$pps_prev);
        fwrite($this->_filehandle,$pps_next);
        fwrite($this->_filehandle,$pps_dir);
        for($i=0; $i < 5; $i++) {
            fwrite($this->_filehandle,$unknown1);
        }
        fwrite($this->_filehandle,$pps_ts1s);
        fwrite($this->_filehandle,$pps_ts1d);
        fwrite($this->_filehandle,$pps_ts2d);
        fwrite($this->_filehandle,$pps_ts2d);
        fwrite($this->_filehandle,$pps_sb);
        fwrite($this->_filehandle,$pps_size);
        fwrite($this->_filehandle,$unknown1);
    }

    /**
    * Pad the end of the file
    */
    function _write_padding()
    {
        $biffsize = $this->_biffsize;
        if ($biffsize < 4096) {
	    $min_size = 4096;
        }
	else {    
            $min_size = 512;
        }
	if ($biffsize % $min_size != 0)
        {
            $padding  = $min_size - ($biffsize % $min_size);
            for($i=0; $i < $padding; $i++) {
                fwrite($this->_filehandle,"\0");
            }
        }
    }
}

/**
* Class for generating Excel XF records (formats)
*
* @author Xavier Noguer <xnoguer@rezebra.com>
* @package Spreadsheet_WriteExcel
*/

class Format
{
  /**
  * Constructor
  *
  * @access public
  * @param integer $index the XF index for the format.
  * @param array   $properties array with properties to be set on initialization.
  */
    function Format($index = 0,$properties =  array())
    {
        $this->xf_index       = $index;
    
        $this->font_index     = 0;
        $this->font           = 'Arial';
        $this->size           = 10;
        $this->bold           = 0x0190;
        $this->_italic        = 0;
        $this->color          = 0x7FFF;
        $this->_underline     = 0;
        $this->font_strikeout = 0;
        $this->font_outline   = 0;
        $this->font_shadow    = 0;
        $this->font_script    = 0;
        $this->font_family    = 0;
        $this->font_charset   = 0;
    
        $this->_num_format    = 0;
    
        $this->hidden         = 0;
        $this->locked         = 1;
    
        $this->_text_h_align  = 0;
        $this->_text_wrap     = 0;
        $this->text_v_align   = 2;
        $this->text_justlast  = 0;
        $this->rotation       = 0;
    
        $this->fg_color       = 0x40;
        $this->bg_color       = 0x41;
    
        $this->pattern        = 0;
    
        $this->bottom         = 0;
        $this->top            = 0;
        $this->left           = 0;
        $this->right          = 0;
    
        $this->bottom_color   = 0x40;
        $this->top_color      = 0x40;
        $this->left_color     = 0x40;
        $this->right_color    = 0x40;
    
        // Set properties passed to Workbook::add_format()
        foreach($properties as $property => $value)
        {
            if(method_exists($this,"set_$property"))
            {
                $aux = 'set_'.$property;
                $this->$aux($value);
            }
        }
    }
    
    /**
    * Generate an Excel BIFF XF record (style or cell).
    *
    * @param string $style The type of the XF record ('style' or 'cell').
    * @return string The XF record
    */
    function get_xf($style)
    {
        // Set the type of the XF record and some of the attributes.
        if ($style == "style") {
            $style = 0xFFF5;
        }
        else {
            $style   = $this->locked;
            $style  |= $this->hidden << 1;
        }
    
        // Flags to indicate if attributes have been set.
        $atr_num     = ($this->_num_format != 0)?1:0;
        $atr_fnt     = ($this->font_index != 0)?1:0;
        $atr_alc     = ($this->_text_wrap)?1:0;
        $atr_bdr     = ($this->bottom   ||
                        $this->top      ||
                        $this->left     ||
                        $this->right)?1:0;
        $atr_pat     = (($this->fg_color != 0x40) ||
                        ($this->bg_color != 0x41) ||
                        $this->pattern)?1:0;
        $atr_prot    = 0;
    
        // Zero the default border colour if the border has not been set.
        if ($this->bottom == 0) {
            $this->bottom_color = 0;
            }
        if ($this->top  == 0) {
            $this->top_color = 0;
            }
        if ($this->right == 0) {
            $this->right_color = 0;
            }
        if ($this->left == 0) {
            $this->left_color = 0;
            }
    
        $record         = 0x00E0;              // Record identifier
        $length         = 0x0010;              // Number of bytes to follow
                                               
        $ifnt           = $this->font_index;   // Index to FONT record
        $ifmt           = $this->_num_format;  // Index to FORMAT record
    
        $align          = $this->_text_h_align;       // Alignment
        $align         |= $this->_text_wrap    << 3;
        $align         |= $this->text_v_align  << 4;
        $align         |= $this->text_justlast << 7;
        $align         |= $this->rotation      << 8;
        $align         |= $atr_num                << 10;
        $align         |= $atr_fnt                << 11;
        $align         |= $atr_alc                << 12;
        $align         |= $atr_bdr                << 13;
        $align         |= $atr_pat                << 14;
        $align         |= $atr_prot               << 15;
    
        $icv            = $this->fg_color;           // fg and bg pattern colors
        $icv           |= $this->bg_color      << 7;
    
        $fill           = $this->pattern;            // Fill and border line style
        $fill          |= $this->bottom        << 6;
        $fill          |= $this->bottom_color  << 9;
    
        $border1        = $this->top;                // Border line style and color
        $border1       |= $this->left          << 3;
        $border1       |= $this->right         << 6;
        $border1       |= $this->top_color     << 9;

        $border2        = $this->left_color;         // Border color
        $border2       |= $this->right_color   << 7;
    
        $header      = pack("vv",       $record, $length);
        $data        = pack("vvvvvvvv", $ifnt, $ifmt, $style, $align,
                                        $icv, $fill,
                                        $border1, $border2);
        return($header.$data);
    }
    
    /**
    * Generate an Excel BIFF FONT record.
    *
    * @see Workbook::_store_all_fonts()
    * @return string The FONT record
    */
    function get_font()
    {
        $dyHeight   = $this->size * 20;    // Height of font (1/20 of a point)
        $icv        = $this->color;        // Index to color palette
        $bls        = $this->bold;         // Bold style
        $sss        = $this->font_script;  // Superscript/subscript
        $uls        = $this->_underline;   // Underline
        $bFamily    = $this->font_family;  // Font family
        $bCharSet   = $this->font_charset; // Character set
        $rgch       = $this->font;         // Font name
    
        $cch        = strlen($rgch);       // Length of font name
        $record     = 0x31;                // Record identifier
        $length     = 0x0F + $cch;         // Record length
        $reserved   = 0x00;                // Reserved
        $grbit      = 0x00;                // Font attributes
        if ($this->_italic) {
            $grbit     |= 0x02;
        }
        if ($this->font_strikeout) {
            $grbit     |= 0x08;
        }
        if ($this->font_outline) {
            $grbit     |= 0x10;
        }
        if ($this->font_shadow) {
            $grbit     |= 0x20;
        }
    
        $header  = pack("vv",         $record, $length);
        $data    = pack("vvvvvCCCCC", $dyHeight, $grbit, $icv, $bls,
                                      $sss, $uls, $bFamily,
                                      $bCharSet, $reserved, $cch);
        return($header . $data. $this->font);
    }
    
    /**
    * Returns a unique hash key for a font. Used by Workbook->_store_all_fonts()
    *
    * The elements that form the key are arranged to increase the probability of
    * generating a unique key. Elements that hold a large range of numbers
    * (eg. _color) are placed between two binary elements such as _italic
    *
    * @return string A key for this font
    */
    function get_font_key()
    {
        $key  = "$this->font$this->size";
        $key .= "$this->font_script$this->_underline";
        $key .= "$this->font_strikeout$this->bold$this->font_outline";
        $key .= "$this->font_family$this->font_charset";
        $key .= "$this->font_shadow$this->color$this->_italic";
        $key  = str_replace(" ","_",$key);
        return ($key);
    }
    
    /**
    * Returns the index used by Worksheet->_XF()
    *
    * @return integer The index for the XF record
    */
    function get_xf_index()
    {
        return($this->xf_index);
    }
    
    /**
    * Used in conjunction with the set_xxx_color methods to convert a color
    * string into a number. Color range is 0..63 but we will restrict it
    * to 8..63 to comply with Gnumeric. Colors 0..7 are repeated in 8..15.
    *
    * @param string $name_color name of the color (i.e.: 'blue', 'red', etc..). Optional.
    * @return integer The color index
    */
    function _get_color($name_color = '')
    {
        $colors = array(
                        'aqua'    => 0x0F,
                        'cyan'    => 0x0F,
                        'black'   => 0x08,
                        'blue'    => 0x0C,
                        'brown'   => 0x10,
                        'magenta' => 0x0E,
                        'fuchsia' => 0x0E,
                        'gray'    => 0x17,
                        'grey'    => 0x17,
                        'green'   => 0x11,
                        'lime'    => 0x0B,
                        'navy'    => 0x12,
                        'orange'  => 0x35,
                        'purple'  => 0x14,
                        'red'     => 0x0A,
                        'silver'  => 0x16,
                        'white'   => 0x09,
                        'yellow'  => 0x0D
                       );
    
        // Return the default color, 0x7FFF, if undef,
        if($name_color == '') {
            return(0x7FFF);
        }
    
        // or the color string converted to an integer,
        if(isset($colors[$name_color])) {
            return($colors[$name_color]);
        }
    
        // or the default color if string is unrecognised,
        if(preg_match("/\D/",$name_color)) {
            return(0x7FFF);
        }
    
        // or an index < 8 mapped into the correct range,
        if($name_color < 8) {
            return($name_color + 8);
        }
    
        // or the default color if arg is outside range,
        if($name_color > 63) {
            return(0x7FFF);
        }
    
        // or an integer in the valid range
        return($name_color);
    }
    
    /**
    * Set cell alignment.
    *
    * @access public
    * @param string $location alignment for the cell ('left', 'right', etc...).
    */
    function set_align($location)
    {
        if (preg_match("/\d/",$location)) {
            return;                      // Ignore numbers
        }
    
        $location = strtolower($location);
    
        if ($location == 'left')
            $this->_text_h_align = 1; 
        if ($location == 'centre')
            $this->_text_h_align = 2; 
        if ($location == 'center')
            $this->_text_h_align = 2; 
        if ($location == 'right')
            $this->_text_h_align = 3; 
        if ($location == 'fill')
            $this->_text_h_align = 4; 
        if ($location == 'justify')
            $this->_text_h_align = 5;
        if ($location == 'merge')
            $this->_text_h_align = 6;
        if ($location == 'equal_space') // For T.K.
            $this->_text_h_align = 7; 
        if ($location == 'top')
            $this->text_v_align = 0; 
        if ($location == 'vcentre')
            $this->text_v_align = 1; 
        if ($location == 'vcenter')
            $this->text_v_align = 1; 
        if ($location == 'bottom')
            $this->text_v_align = 2; 
        if ($location == 'vjustify')
            $this->text_v_align = 3; 
        if ($location == 'vequal_space') // For T.K.
            $this->text_v_align = 4; 
    }
    
    /**
    * This is an alias for the unintuitive set_align('merge')
    *
    * @access public
    */
    function set_merge()
    {
        $this->set_align('merge');
    }
    
    /**
    * Bold has a range 0x64..0x3E8.
    * 0x190 is normal. 0x2BC is bold.
    *
    * @access public
    * @param integer $weight Weight for the text, 0 maps to 0x190, 1 maps to 0x2BC. 
                             It's Optional, default is 1 (bold).
    */
    function set_bold($weight = 1)
    {
        if($weight == 1) {
            $weight = 0x2BC;  // Bold text
        }
        if($weight == 0) {
            $weight = 0x190;  // Normal text
        }
        if($weight <  0x064) {
            $weight = 0x190;  // Lower bound
        }
        if($weight >  0x3E8) {
            $weight = 0x190;  // Upper bound
        }
        $this->bold = $weight;
    }
    
    
    /************************************
    * FUNCTIONS FOR SETTING CELLS BORDERS
    */
    
    /**
    * Sets the bottom border of the cell
    *
    * @access public
    * @param integer $style style of the cell border. 1 => thin, 2 => thick.
    */
    function set_bottom($style)
    {
        $this->bottom = $style;
    }
    
    /**
    * Sets the top border of the cell
    *
    * @access public
    * @param integer $style style of the cell top border. 1 => thin, 2 => thick.
    */
    function set_top($style)
    {
        $this->top = $style;
    }
    
    /**
    * Sets the left border of the cell
    *
    * @access public
    * @param integer $style style of the cell left border. 1 => thin, 2 => thick.
    */
    function set_left($style)
    {
        $this->left = $style;
    }
    
    /**
    * Sets the right border of the cell
    *
    * @access public
    * @param integer $style style of the cell right border. 1 => thin, 2 => thick.
    */
    function set_right($style)
    {
        $this->right = $style;
    }
    
    
    /**
    * Set cells borders to the same style
    *
    * @access public
    * @param integer $style style to apply for all cell borders. 1 => thin, 2 => thick.
    */
    function set_border($style)
    {
        $this->set_bottom($style);
        $this->set_top($style);
        $this->set_left($style);
        $this->set_right($style);
    }
    
    
    /*******************************************
    * FUNCTIONS FOR SETTING CELLS BORDERS COLORS
    */
    
    /**
    * Sets all the cell's borders to the same color
    *
    * @access public
    * @param mixed $color The color we are setting. Either a string (like 'blue'), 
    *                     or an integer (like 0x41).
    */
    function set_border_color($color)
    {
        $this->set_bottom_color($color);
        $this->set_top_color($color);
        $this->set_left_color($color);
        $this->set_right_color($color);
    }
    
    /**
    * Sets the cell's bottom border color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (range is [8...63]).
    */
    function set_bottom_color($color)
    {
        $value = $this->_get_color($color);
        $this->bottom_color = $value;
    }
    
    /**
    * Sets the cell's top border color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (range is [8...63]).
    */
    function set_top_color($color)
    {
        $value = $this->_get_color($color);
        $this->top_color = $value;
    }
    
    /**
    * Sets the cell's left border color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (like 0x41).
    */
    function set_left_color($color)
    {
        $value = $this->_get_color($color);
        $this->left_color = $value;
    }
    
    /**
    * Sets the cell's right border color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (like 0x41).
    */
    function set_right_color($color)
    {
        $value = $this->_get_color($color);
        $this->right_color = $value;
    }
    
    
    /**
    * Sets the cell's foreground color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (like 0x41).
    */
    function set_fg_color($color)
    {
        $value = $this->_get_color($color);
        $this->fg_color = $value;
    }
      
    /**
    * Sets the cell's background color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (like 0x41).
    */
    function set_bg_color($color)
    {
        $value = $this->_get_color($color);
        $this->bg_color = $value;
    }
    
    /**
    * Sets the cell's color
    *
    * @access public
    * @param mixed $color either a string (like 'blue'), or an integer (like 0x41).
    */
    function set_color($color)
    {
        $value = $this->_get_color($color);
        $this->color = $value;
    }
    
    /**
    * Sets the pattern attribute of a cell
    *
    * @access public
    * @param integer $arg Optional. Defaults to 1.
    */
    function set_pattern($arg = 1)
    {
        $this->pattern = $arg;
    }
    
    /**
    * Sets the underline of the text
    *
    * @access public
    * @param integer $underline The value for underline. Possible values are:
    *                          1 => underline, 2 => double underline.
    */
    function set_underline($underline)
    {
        $this->_underline = $underline;
    }
 
    /**
    * Sets the font style as italic
    *
    * @access public
    */
    function set_italic()
    {
        $this->_italic = 1;
    }

    /**
    * Sets the font size 
    *
    * @access public
    * @param integer $size The font size (in pixels I think).
    */
    function set_size($size)
    {
        $this->size = $size;
    }
    
    /**
    * Sets the num format
    *
    * @access public
    * @param integer $num_format The num format.
    */
    function set_num_format($num_format)
    {
        $this->_num_format = $num_format;
    }
    
    /**
    * Sets text wrapping
    *
    * @access public
    * @param integer $text_wrap Optional. 0 => no text wrapping, 1 => text wrapping. 
    *                           Defaults to 1.
    */
    function set_text_wrap($text_wrap = 1)
    {
        $this->_text_wrap = $text_wrap;
    }
}


/**
* Class for parsing Excel formulas
*
* @author Xavier Noguer <xnoguer@rezebra.com>
* @package Spreadsheet_WriteExcel
*/
class Parser
  {
/**
* The class constructor
*
* @param integer $byte_order The byte order (Little endian or Big endian) of the architecture
                             (optional). 1 => big endian, 0 (default) => little endian. 
*/
  function Parser($byte_order = 0)
    {
    $this->_current_char  = 0;        // The index of the character we are currently looking at.
    $this->_current_token = '';       // The token we are working on.
    $this->_formula       = "";       // The formula to parse.
    $this->_lookahead     = '';       // The character ahead of the current char.
    $this->_parse_tree    = '';       // The parse tree to be generated.
    $this->_initialize_hashes();      // Initialize the hashes: ptg's and function's ptg's
    $this->_byte_order = $byte_order; // Little Endian or Big Endian
    $this->_func_args  = 0;           // Number of arguments for the current function
    $this->_volatile   = 0;
    }

/**
* Initialize the ptg and function hashes. 
*/
  function _initialize_hashes()
    {
    // The Excel ptg indices
    $this->ptg = array(
        'ptgExp'       => 0x01,
        'ptgTbl'       => 0x02,
        'ptgAdd'       => 0x03,
        'ptgSub'       => 0x04,
        'ptgMul'       => 0x05,
        'ptgDiv'       => 0x06,
        'ptgPower'     => 0x07,
        'ptgConcat'    => 0x08,
        'ptgLT'        => 0x09,
        'ptgLE'        => 0x0A,
        'ptgEQ'        => 0x0B,
        'ptgGE'        => 0x0C,
        'ptgGT'        => 0x0D,
        'ptgNE'        => 0x0E,
        'ptgIsect'     => 0x0F,
        'ptgUnion'     => 0x10,
        'ptgRange'     => 0x11,
        'ptgUplus'     => 0x12,
        'ptgUminus'    => 0x13,
        'ptgPercent'   => 0x14,
        'ptgParen'     => 0x15,
        'ptgMissArg'   => 0x16,
        'ptgStr'       => 0x17,
        'ptgAttr'      => 0x19,
        'ptgSheet'     => 0x1A,
        'ptgEndSheet'  => 0x1B,
        'ptgErr'       => 0x1C,
        'ptgBool'      => 0x1D,
        'ptgInt'       => 0x1E,
        'ptgNum'       => 0x1F,
        'ptgArray'     => 0x20,
        'ptgFunc'      => 0x21,
        'ptgFuncVar'   => 0x22,
        'ptgName'      => 0x23,
        'ptgRef'       => 0x24,
        'ptgArea'      => 0x25,
        'ptgMemArea'   => 0x26,
        'ptgMemErr'    => 0x27,
        'ptgMemNoMem'  => 0x28,
        'ptgMemFunc'   => 0x29,
        'ptgRefErr'    => 0x2A,
        'ptgAreaErr'   => 0x2B,
        'ptgRefN'      => 0x2C,
        'ptgAreaN'     => 0x2D,
        'ptgMemAreaN'  => 0x2E,
        'ptgMemNoMemN' => 0x2F,
        'ptgNameX'     => 0x39,
        'ptgRef3d'     => 0x3A,
        'ptgArea3d'    => 0x3B,
        'ptgRefErr3d'  => 0x3C,
        'ptgAreaErr3d' => 0x3D,
        'ptgArrayV'    => 0x40,
        'ptgFuncV'     => 0x41,
        'ptgFuncVarV'  => 0x42,
        'ptgNameV'     => 0x43,
        'ptgRefV'      => 0x44,
        'ptgAreaV'     => 0x45,
        'ptgMemAreaV'  => 0x46,
        'ptgMemErrV'   => 0x47,
        'ptgMemNoMemV' => 0x48,
        'ptgMemFuncV'  => 0x49,
        'ptgRefErrV'   => 0x4A,
        'ptgAreaErrV'  => 0x4B,
        'ptgRefNV'     => 0x4C,
        'ptgAreaNV'    => 0x4D,
        'ptgMemAreaNV' => 0x4E,
        'ptgMemNoMemN' => 0x4F,
        'ptgFuncCEV'   => 0x58,
        'ptgNameXV'    => 0x59,
        'ptgRef3dV'    => 0x5A,
        'ptgArea3dV'   => 0x5B,
        'ptgRefErr3dV' => 0x5C,
        'ptgAreaErr3d' => 0x5D,
        'ptgArrayA'    => 0x60,
        'ptgFuncA'     => 0x61,
        'ptgFuncVarA'  => 0x62,
        'ptgNameA'     => 0x63,
        'ptgRefA'      => 0x64,
        'ptgAreaA'     => 0x65,
        'ptgMemAreaA'  => 0x66,
        'ptgMemErrA'   => 0x67,
        'ptgMemNoMemA' => 0x68,
        'ptgMemFuncA'  => 0x69,
        'ptgRefErrA'   => 0x6A,
        'ptgAreaErrA'  => 0x6B,
        'ptgRefNA'     => 0x6C,
        'ptgAreaNA'    => 0x6D,
        'ptgMemAreaNA' => 0x6E,
        'ptgMemNoMemN' => 0x6F,
        'ptgFuncCEA'   => 0x78,
        'ptgNameXA'    => 0x79,
        'ptgRef3dA'    => 0x7A,
        'ptgArea3dA'   => 0x7B,
        'ptgRefErr3dA' => 0x7C,
        'ptgAreaErr3d' => 0x7D
        );

    // Thanks to Michael Meeks and Gnumeric for the initial arg values.
    //
    // The following hash was generated by "function_locale.pl" in the distro.
    // Refer to function_locale.pl for non-English function names.
    //
    // The array elements are as follow:
    // ptg:   The Excel function ptg code.
    // args:  The number of arguments that the function takes:
    //           >=0 is a fixed number of arguments.
    //           -1  is a variable  number of arguments.
    // class: The reference, value or array class of the function args.
    // vol:   The function is volatile.
    //
    $this->_functions = array(
          // function                  ptg  args  class  vol
          'COUNT'           => array(   0,   -1,    0,    0 ),
          'IF'              => array(   1,   -1,    1,    0 ),
          'ISNA'            => array(   2,    1,    1,    0 ),
          'ISERROR'         => array(   3,    1,    1,    0 ),
          'SUM'             => array(   4,   -1,    0,    0 ),
          'AVERAGE'         => array(   5,   -1,    0,    0 ),
          'MIN'             => array(   6,   -1,    0,    0 ),
          'MAX'             => array(   7,   -1,    0,    0 ),
          'ROW'             => array(   8,   -1,    0,    0 ),
          'COLUMN'          => array(   9,   -1,    0,    0 ),
          'NA'              => array(  10,    0,    0,    0 ),
          'NPV'             => array(  11,   -1,    1,    0 ),
          'STDEV'           => array(  12,   -1,    0,    0 ),
          'DOLLAR'          => array(  13,   -1,    1,    0 ),
          'FIXED'           => array(  14,   -1,    1,    0 ),
          'SIN'             => array(  15,    1,    1,    0 ),
          'COS'             => array(  16,    1,    1,    0 ),
          'TAN'             => array(  17,    1,    1,    0 ),
          'ATAN'            => array(  18,    1,    1,    0 ),
          'PI'              => array(  19,    0,    1,    0 ),
          'SQRT'            => array(  20,    1,    1,    0 ),
          'EXP'             => array(  21,    1,    1,    0 ),
          'LN'              => array(  22,    1,    1,    0 ),
          'LOG10'           => array(  23,    1,    1,    0 ),
          'ABS'             => array(  24,    1,    1,    0 ),
          'INT'             => array(  25,    1,    1,    0 ),
          'SIGN'            => array(  26,    1,    1,    0 ),
          'ROUND'           => array(  27,    2,    1,    0 ),
          'LOOKUP'          => array(  28,   -1,    0,    0 ),
          'INDEX'           => array(  29,   -1,    0,    1 ),
          'REPT'            => array(  30,    2,    1,    0 ),
          'MID'             => array(  31,    3,    1,    0 ),
          'LEN'             => array(  32,    1,    1,    0 ),
          'VALUE'           => array(  33,    1,    1,    0 ),
          'TRUE'            => array(  34,    0,    1,    0 ),
          'FALSE'           => array(  35,    0,    1,    0 ),
          'AND'             => array(  36,   -1,    0,    0 ),
          'OR'              => array(  37,   -1,    0,    0 ),
          'NOT'             => array(  38,    1,    1,    0 ),
          'MOD'             => array(  39,    2,    1,    0 ),
          'DCOUNT'          => array(  40,    3,    0,    0 ),
          'DSUM'            => array(  41,    3,    0,    0 ),
          'DAVERAGE'        => array(  42,    3,    0,    0 ),
          'DMIN'            => array(  43,    3,    0,    0 ),
          'DMAX'            => array(  44,    3,    0,    0 ),
          'DSTDEV'          => array(  45,    3,    0,    0 ),
          'VAR'             => array(  46,   -1,    0,    0 ),
          'DVAR'            => array(  47,    3,    0,    0 ),
          'TEXT'            => array(  48,    2,    1,    0 ),
          'LINEST'          => array(  49,   -1,    0,    0 ),
          'TREND'           => array(  50,   -1,    0,    0 ),
          'LOGEST'          => array(  51,   -1,    0,    0 ),
          'GROWTH'          => array(  52,   -1,    0,    0 ),
          'PV'              => array(  56,   -1,    1,    0 ),
          'FV'              => array(  57,   -1,    1,    0 ),
          'NPER'            => array(  58,   -1,    1,    0 ),
          'PMT'             => array(  59,   -1,    1,    0 ),
          'RATE'            => array(  60,   -1,    1,    0 ),
          'MIRR'            => array(  61,    3,    0,    0 ),
          'IRR'             => array(  62,   -1,    0,    0 ),
          'RAND'            => array(  63,    0,    1,    1 ),
          'MATCH'           => array(  64,   -1,    0,    0 ),
          'DATE'            => array(  65,    3,    1,    0 ),
          'TIME'            => array(  66,    3,    1,    0 ),
          'DAY'             => array(  67,    1,    1,    0 ),
          'MONTH'           => array(  68,    1,    1,    0 ),
          'YEAR'            => array(  69,    1,    1,    0 ),
          'WEEKDAY'         => array(  70,   -1,    1,    0 ),
          'HOUR'            => array(  71,    1,    1,    0 ),
          'MINUTE'          => array(  72,    1,    1,    0 ),
          'SECOND'          => array(  73,    1,    1,    0 ),
          'NOW'             => array(  74,    0,    1,    1 ),
          'AREAS'           => array(  75,    1,    0,    1 ),
          'ROWS'            => array(  76,    1,    0,    1 ),
          'COLUMNS'         => array(  77,    1,    0,    1 ),
          'OFFSET'          => array(  78,   -1,    0,    1 ),
          'SEARCH'          => array(  82,   -1,    1,    0 ),
          'TRANSPOSE'       => array(  83,    1,    1,    0 ),
          'TYPE'            => array(  86,    1,    1,    0 ),
          'ATAN2'           => array(  97,    2,    1,    0 ),
          'ASIN'            => array(  98,    1,    1,    0 ),
          'ACOS'            => array(  99,    1,    1,    0 ),
          'CHOOSE'          => array( 100,   -1,    1,    0 ),
          'HLOOKUP'         => array( 101,   -1,    0,    0 ),
          'VLOOKUP'         => array( 102,   -1,    0,    0 ),
          'ISREF'           => array( 105,    1,    0,    0 ),
          'LOG'             => array( 109,   -1,    1,    0 ),
          'CHAR'            => array( 111,    1,    1,    0 ),
          'LOWER'           => array( 112,    1,    1,    0 ),
          'UPPER'           => array( 113,    1,    1,    0 ),
          'PROPER'          => array( 114,    1,    1,    0 ),
          'LEFT'            => array( 115,   -1,    1,    0 ),
          'RIGHT'           => array( 116,   -1,    1,    0 ),
          'EXACT'           => array( 117,    2,    1,    0 ),
          'TRIM'            => array( 118,    1,    1,    0 ),
          'REPLACE'         => array( 119,    4,    1,    0 ),
          'SUBSTITUTE'      => array( 120,   -1,    1,    0 ),
          'CODE'            => array( 121,    1,    1,    0 ),
          'FIND'            => array( 124,   -1,    1,    0 ),
          'CELL'            => array( 125,   -1,    0,    1 ),
          'ISERR'           => array( 126,    1,    1,    0 ),
          'ISTEXT'          => array( 127,    1,    1,    0 ),
          'ISNUMBER'        => array( 128,    1,    1,    0 ),
          'ISBLANK'         => array( 129,    1,    1,    0 ),
          'T'               => array( 130,    1,    0,    0 ),
          'N'               => array( 131,    1,    0,    0 ),
          'DATEVALUE'       => array( 140,    1,    1,    0 ),
          'TIMEVALUE'       => array( 141,    1,    1,    0 ),
          'SLN'             => array( 142,    3,    1,    0 ),
          'SYD'             => array( 143,    4,    1,    0 ),
          'DDB'             => array( 144,   -1,    1,    0 ),
          'INDIRECT'        => array( 148,   -1,    1,    1 ),
          'CALL'            => array( 150,   -1,    1,    0 ),
          'CLEAN'           => array( 162,    1,    1,    0 ),
          'MDETERM'         => array( 163,    1,    2,    0 ),
          'MINVERSE'        => array( 164,    1,    2,    0 ),
          'MMULT'           => array( 165,    2,    2,    0 ),
          'IPMT'            => array( 167,   -1,    1,    0 ),
          'PPMT'            => array( 168,   -1,    1,    0 ),
          'COUNTA'          => array( 169,   -1,    0,    0 ),
          'PRODUCT'         => array( 183,   -1,    0,    0 ),
          'FACT'            => array( 184,    1,    1,    0 ),
          'DPRODUCT'        => array( 189,    3,    0,    0 ),
          'ISNONTEXT'       => array( 190,    1,    1,    0 ),
          'STDEVP'          => array( 193,   -1,    0,    0 ),
          'VARP'            => array( 194,   -1,    0,    0 ),
          'DSTDEVP'         => array( 195,    3,    0,    0 ),
          'DVARP'           => array( 196,    3,    0,    0 ),
          'TRUNC'           => array( 197,   -1,    1,    0 ),
          'ISLOGICAL'       => array( 198,    1,    1,    0 ),
          'DCOUNTA'         => array( 199,    3,    0,    0 ),
          'ROUNDUP'         => array( 212,    2,    1,    0 ),
          'ROUNDDOWN'       => array( 213,    2,    1,    0 ),
          'RANK'            => array( 216,   -1,    0,    0 ),
          'ADDRESS'         => array( 219,   -1,    1,    0 ),
          'DAYS360'         => array( 220,   -1,    1,    0 ),
          'TODAY'           => array( 221,    0,    1,    1 ),
          'VDB'             => array( 222,   -1,    1,    0 ),
          'MEDIAN'          => array( 227,   -1,    0,    0 ),
          'SUMPRODUCT'      => array( 228,   -1,    2,    0 ),
          'SINH'            => array( 229,    1,    1,    0 ),
          'COSH'            => array( 230,    1,    1,    0 ),
          'TANH'            => array( 231,    1,    1,    0 ),
          'ASINH'           => array( 232,    1,    1,    0 ),
          'ACOSH'           => array( 233,    1,    1,    0 ),
          'ATANH'           => array( 234,    1,    1,    0 ),
          'DGET'            => array( 235,    3,    0,    0 ),
          'INFO'            => array( 244,    1,    1,    1 ),
          'DB'              => array( 247,   -1,    1,    0 ),
          'FREQUENCY'       => array( 252,    2,    0,    0 ),
          'ERROR.TYPE'      => array( 261,    1,    1,    0 ),
          'REGISTER.ID'     => array( 267,   -1,    1,    0 ),
          'AVEDEV'          => array( 269,   -1,    0,    0 ),
          'BETADIST'        => array( 270,   -1,    1,    0 ),
          'GAMMALN'         => array( 271,    1,    1,    0 ),
          'BETAINV'         => array( 272,   -1,    1,    0 ),
          'BINOMDIST'       => array( 273,    4,    1,    0 ),
          'CHIDIST'         => array( 274,    2,    1,    0 ),
          'CHIINV'          => array( 275,    2,    1,    0 ),
          'COMBIN'          => array( 276,    2,    1,    0 ),
          'CONFIDENCE'      => array( 277,    3,    1,    0 ),
          'CRITBINOM'       => array( 278,    3,    1,    0 ),
          'EVEN'            => array( 279,    1,    1,    0 ),
          'EXPONDIST'       => array( 280,    3,    1,    0 ),
          'FDIST'           => array( 281,    3,    1,    0 ),
          'FINV'            => array( 282,    3,    1,    0 ),
          'FISHER'          => array( 283,    1,    1,    0 ),
          'FISHERINV'       => array( 284,    1,    1,    0 ),
          'FLOOR'           => array( 285,    2,    1,    0 ),
          'GAMMADIST'       => array( 286,    4,    1,    0 ),
          'GAMMAINV'        => array( 287,    3,    1,    0 ),
          'CEILING'         => array( 288,    2,    1,    0 ),
          'HYPGEOMDIST'     => array( 289,    4,    1,    0 ),
          'LOGNORMDIST'     => array( 290,    3,    1,    0 ),
          'LOGINV'          => array( 291,    3,    1,    0 ),
          'NEGBINOMDIST'    => array( 292,    3,    1,    0 ),
          'NORMDIST'        => array( 293,    4,    1,    0 ),
          'NORMSDIST'       => array( 294,    1,    1,    0 ),
          'NORMINV'         => array( 295,    3,    1,    0 ),
          'NORMSINV'        => array( 296,    1,    1,    0 ),
          'STANDARDIZE'     => array( 297,    3,    1,    0 ),
          'ODD'             => array( 298,    1,    1,    0 ),
          'PERMUT'          => array( 299,    2,    1,    0 ),
          'POISSON'         => array( 300,    3,    1,    0 ),
          'TDIST'           => array( 301,    3,    1,    0 ),
          'WEIBULL'         => array( 302,    4,    1,    0 ),
          'SUMXMY2'         => array( 303,    2,    2,    0 ),
          'SUMX2MY2'        => array( 304,    2,    2,    0 ),
          'SUMX2PY2'        => array( 305,    2,    2,    0 ),
          'CHITEST'         => array( 306,    2,    2,    0 ),
          'CORREL'          => array( 307,    2,    2,    0 ),
          'COVAR'           => array( 308,    2,    2,    0 ),
          'FORECAST'        => array( 309,    3,    2,    0 ),
          'FTEST'           => array( 310,    2,    2,    0 ),
          'INTERCEPT'       => array( 311,    2,    2,    0 ),
          'PEARSON'         => array( 312,    2,    2,    0 ),
          'RSQ'             => array( 313,    2,    2,    0 ),
          'STEYX'           => array( 314,    2,    2,    0 ),
          'SLOPE'           => array( 315,    2,    2,    0 ),
          'TTEST'           => array( 316,    4,    2,    0 ),
          'PROB'            => array( 317,   -1,    2,    0 ),
          'DEVSQ'           => array( 318,   -1,    0,    0 ),
          'GEOMEAN'         => array( 319,   -1,    0,    0 ),
          'HARMEAN'         => array( 320,   -1,    0,    0 ),
          'SUMSQ'           => array( 321,   -1,    0,    0 ),
          'KURT'            => array( 322,   -1,    0,    0 ),
          'SKEW'            => array( 323,   -1,    0,    0 ),
          'ZTEST'           => array( 324,   -1,    0,    0 ),
          'LARGE'           => array( 325,    2,    0,    0 ),
          'SMALL'           => array( 326,    2,    0,    0 ),
          'QUARTILE'        => array( 327,    2,    0,    0 ),
          'PERCENTILE'      => array( 328,    2,    0,    0 ),
          'PERCENTRANK'     => array( 329,   -1,    0,    0 ),
          'MODE'            => array( 330,   -1,    2,    0 ),
          'TRIMMEAN'        => array( 331,    2,    0,    0 ),
          'TINV'            => array( 332,    2,    1,    0 ),
          'CONCATENATE'     => array( 336,   -1,    1,    0 ),
          'POWER'           => array( 337,    2,    1,    0 ),
          'RADIANS'         => array( 342,    1,    1,    0 ),
          'DEGREES'         => array( 343,    1,    1,    0 ),
          'SUBTOTAL'        => array( 344,   -1,    0,    0 ),
          'SUMIF'           => array( 345,   -1,    0,    0 ),
          'COUNTIF'         => array( 346,    2,    0,    0 ),
          'COUNTBLANK'      => array( 347,    1,    0,    0 ),
          'ROMAN'           => array( 354,   -1,    1,    0 )
          );
    }

/**
* Convert a token to the proper ptg value.
*
* @param mixed $token The token to convert.
*/
  function _convert($token)
    {
    if(is_numeric($token))
        {
        return($this->_convert_number($token));
        }
    // match references like A1
    elseif(preg_match("/^([A-I]?[A-Z])(\d+)$/",$token))
        {
        return($this->_convert_ref2d($token));
        }
    // match ranges like A1:B2
    elseif(preg_match("/^([A-I]?[A-Z])(\d+)\:([A-I]?[A-Z])(\d+)$/",$token))
        {
        return($this->_convert_range2d($token));
        }
    // match ranges like A1..B2
    elseif(preg_match("/^([A-I]?[A-Z])(\d+)\.\.([A-I]?[A-Z])(\d+)$/",$token))
        {
        return($this->_convert_range2d($token));
        }
    elseif(isset($this->ptg[$token])) // operators (including parentheses)
        {
        return(pack("C", $this->ptg[$token]));
        }
    elseif(preg_match("/[A-Z0-9À-Ü\.]+/",$token))
        {
        return($this->_convert_function($token,$this->_func_args));
        }
    // if it's an argument, ignore the token (the argument remains)
    elseif($token == 'arg')
        {
        $this->_func_args++;
        return('');
        }
    die("Unknown token $token");
    }

/**
* Convert a number token to ptgInt or ptgNum
*
* @param mixed $num an integer or double for conersion to its ptg value
*/
  function _convert_number($num)
    {
    // Integer in the range 0..2**16-1
    if ((preg_match("/^\d+$/",$num)) and ($num <= 65535)) {
        return pack("Cv", $this->ptg['ptgInt'], $num);
        }
    else // A float
        {
        if($this->_byte_order) // if it's Big Endian
            {
            $num = strrev($num);
            }
        return pack("Cd", $this->ptg['ptgNum'], $num);
        }
    }

/**
* Convert a function to a ptgFunc or ptgFuncVarV depending on the number of
* args that it takes.
*
* @param string  $token    The name of the function for convertion to ptg value.
* @param integer $num_args The number of arguments the function recieves.
*/
  function _convert_function($token, $num_args)
    {
    $this->_func_args = 0; // re initialize the number of arguments
    $args     = $this->_functions[$token][1];
    $volatile = $this->_functions[$token][3];

    if($volatile) {
        $this->_volatile = 1;
        }
    // Fixed number of args eg. TIME($i,$j,$k).
    if ($args >= 0)
        {
        return(pack("Cv", $this->ptg['ptgFuncV'], $this->_functions[$token][0]));
        }
    // Variable number of args eg. SUM($i,$j,$k, ..).
    if ($args == -1) {
        return(pack("CCv", $this->ptg['ptgFuncVarV'], $num_args, $this->_functions[$token][0]));
        }
    }

/**
* Convert an Excel range such as A1:D4 to a ptgRefV.
*
* @param string $range An Excel range in the A1:A2 or A1..A2 format.
*/
  function _convert_range2d($range)
    {
    $class = 2; // as far as I know, this is magick.

    // Split the range into 2 cell refs
    if(preg_match("/^([A-I]?[A-Z])(\d+)\:([A-I]?[A-Z])(\d+)$/",$range)) {
        list($cell1, $cell2) = split(':', $range);
        }
    elseif(preg_match("/^([A-I]?[A-Z])(\d+)\.\.([A-I]?[A-Z])(\d+)$/",$range)) {
        list($cell1, $cell2) = split('\.\.', $range);
        }
    else {
        die("Unknown range separator");
        }

    // Convert the cell references
    list($row1, $col1) = $this->_cell_to_packed_rowcol($cell1);
    list($row2, $col2) = $this->_cell_to_packed_rowcol($cell2);

    // The ptg value depends on the class of the ptg.
    if ($class == 0) {
        $ptgArea = pack("C", $this->ptg['ptgArea']);
        }
    elseif ($class == 1) {
        $ptgArea = pack("C", $this->ptg['ptgAreaV']);
        }
    elseif ($class == 2) {
        $ptgArea = pack("C", $this->ptg['ptgAreaA']);
        }
    else{
        die("Unknown class ");
        }

    return($ptgArea . $row1 . $row2 . $col1. $col2);
    }

/**
* Convert an Excel reference such as A1, $B2, C$3 or $D$4 to a ptgRefV.
*
* @param string $cell An Excel cell reference
*/
  function _convert_ref2d($cell)
    {
    $class = 2; // as far as I know, this is magick.

    // Convert the cell reference
    list($row, $col) = $this->_cell_to_packed_rowcol($cell);

    // The ptg value depends on the class of the ptg.
    if ($class == 0) {
        $ptgRef = pack("C", $this->ptg['ptgRef']);
        }
    elseif ($class == 1) {
        $ptgRef = pack("C", $this->ptg['ptgRefV']);
        }
    elseif ($class == 2) {
        $ptgRef = pack("C", $this->ptg['ptgRefA']);
        }
    else{
        die("Unknown class ");
        }
    return $ptgRef.$row.$col;
    }

/**
* pack() row and column into the required 3 byte format.
*
* @param string $cell The Excel cell reference to be packed
*/
  function _cell_to_packed_rowcol($cell)
    {
    list($row, $col, $row_rel, $col_rel) = $this->_cell_to_rowcol($cell);
    if ($col >= 256) {
        die("Column in: $cell greater than 255 ");
        }
    if ($row >= 16384) {
        die("Row in: $cell greater than 16384 ");
        }

    // Set the high bits to indicate if row or col are relative.
    $row    |= $col_rel << 14;
    $row    |= $row_rel << 15;

    $row     = pack('v', $row);
    $col     = pack('C', $col);

    return (array($row, $col));
    }

/**
* Convert an Excel cell reference such as A1 or $B2 or C$3 or $D$4 to a zero
* indexed row and column number. Also returns two boolean values to indicate
* whether the row or column are relative references.
*
* @param string $cell The Excel cell reference in A1 format.
*/
  function _cell_to_rowcol($cell)
    {
    preg_match('/(\$)?([A-I]?[A-Z])(\$)?(\d+)/',$cell,$match);
    // return absolute column if there is a $ in the ref
    $col_rel = empty($match[1]) ? 1 : 0;
    $col_ref = $match[2];
    $row_rel = empty($match[3]) ? 1 : 0;
    $row     = $match[4];

    // Convert base26 column string to a number.
    $expn   = strlen($col_ref) - 1;
    $col    = 0;
    for($i=0; $i < strlen($col_ref); $i++)
    {
        $col += (ord($col_ref{$i}) - ord('A') + 1) * pow(26, $expn);
        $expn--;
    }

    // Convert 1-index to zero-index
    $row--;
    $col--;

    return(array($row, $col, $row_rel, $col_rel));
    }

/**
* Advance to the next valid token.
*/
  function _advance()
    {
    $i = $this->_current_char;
    // eat up white spaces
    if($i < strlen($this->_formula))
        {
        while($this->_formula{$i} == " ")
            {
            $i++;
            }
        if($i < strlen($this->_formula) - 1)
            {
            $this->_lookahead = $this->_formula{$i+1};
            }
        $token = "";
        }
    while($i < strlen($this->_formula))
        {
        $token .= $this->_formula{$i};
        if($this->_match($token) != '')
            {
            if($i < strlen($this->_formula) - 1)
                {
                $this->_lookahead = $this->_formula{$i+1};
                }
            $this->_current_char = $i + 1;
            $this->_current_token = $token;
            return(1);
            }
        $this->_lookahead = $this->_formula{$i+2};
        $i++;
        }
    //die("Lexical error ".$this->_current_char);
    }

/**
* Checks if it's a valid token.
*
* @param mixed $token The token to check.
*/
  function _match($token)
    {
    switch($token)
        {
        case ADD:
            return($token);
            break;
        case SUB:
            return($token);
            break;
        case MUL:
            return($token);
            break;
        case DIV:
            return($token);
            break;
        case OPEN:
            return($token);
            break;
        case CLOSE:
            return($token);
            break;
        case COMA:
            return($token);
            break;
        default:
	    // if it's a reference
            if(eregi("^[A-I]?[A-Z][0-9]+$",$token) and 
	       !ereg("[0-9]",$this->_lookahead) and 
               ($this->_lookahead != ':') and ($this->_lookahead != '.'))
                {
                return($token);
                }
            // if it's a range (A1:A2)
            elseif(eregi("^[A-I]?[A-Z][0-9]+:[A-I]?[A-Z][0-9]+$",$token) and 
	           !ereg("[0-9]",$this->_lookahead))
	        {
		return($token);
		}
            // if it's a range (A1..A2)
            elseif(eregi("^[A-I]?[A-Z][0-9]+\.\.[A-I]?[A-Z][0-9]+$",$token) and 
	           !ereg("[0-9]",$this->_lookahead))
	        {
		return($token);
		}
            elseif(is_numeric($token) and !is_numeric($token.$this->_lookahead))
                {
                return($token);
                }
            // if it's a function call
            elseif(eregi("^[A-Z0-9À-Ü\.]+$",$token) and ($this->_lookahead == "("))

	        {
		return($token);
		}
            return '';
        }
    }

/**
* The parsing method. It parses a formula.
*
* @access public
* @param string $formula The formula to parse, without the initial equal sign (=).
*/
  function parse($formula)
    {
    $this->_current_char = 0;
    $this->_formula      = $formula;
    $this->_lookahead    = $formula{1};
    $this->_advance();
    $this->_parse_tree   = $this->_expression();
    }

/**
* It parses a expression. It assumes the following rule:
* Expr -> Term [("+" | "-") Term]
*
* @return mixed The parsed ptg'd tree
*/
  function _expression()
    {
    $result = $this->_term();
    while ($this->_current_token == ADD or $this->_current_token == SUB)
        {
        if ($this->_current_token == ADD)
            {
            $this->_advance();
            $result = $this->_create_tree('ptgAdd', $result, $this->_term());
            }
        else 
            {
            $this->_advance();
            $result = $this->_create_tree('ptgSub', $result, $this->_term());
            }
        }
    return $result;
    }

/**
* This function just introduces a ptgParen element in the tree, so that Excel
* doesn't get confused when working with a parenthesized formula afterwards.
*
* @see _fact
* @return mixed The parsed ptg'd tree
*/
  function _parenthesized_expression()
    {
    $result = $this->_create_tree('ptgParen', $this->_expression(), '');
    return($result);
    }

/**
* It parses a term. It assumes the following rule:
* Term -> Fact [("*" | "/") Fact]
*
* @return mixed The parsed ptg'd tree
*/
  function _term()
    {
    $result = $this->_fact();
    while ($this->_current_token == MUL || $this->_current_token == DIV)
        {
        if ($this->_current_token == MUL)
            {
            $this->_advance();
            $result = $this->_create_tree('ptgMul', $result, $this->_fact());
            }
        else 
            {
            $this->_advance();
            $result = $this->_create_tree('ptgDiv', $result, $this->_fact());
            }
        }
    return($result);
    }

/**
* It parses a factor. It assumes the following rule:
* Fact -> ( Expr )
*       | CellRef
*       | CellRange
*       | Number
*       | Function
*
* @return mixed The parsed ptg'd tree
*/
  function _fact()
    {
    if ($this->_current_token == OPEN)
        {
        $this->_advance();         // eat the "("
        $result = $this->_parenthesized_expression();//$this->_expression();

        if ($this->_current_token != CLOSE) {
            die("')' token expected.");
            }
        $this->_advance();         // eat the ")"
        return($result);
        }
    // if it's a reference
    if (eregi("^[A-I]?[A-Z][0-9]+$",$this->_current_token))
        {
        $result = $this->_create_tree($this->_current_token, '', '');
        $this->_advance();
        return($result);
        }
    // if it's a range
    elseif (eregi("^[A-I]?[A-Z][0-9]+:[A-I]?[A-Z][0-9]+$",$this->_current_token) or 
            eregi("^[A-I]?[A-Z][0-9]+\.\.[A-I]?[A-Z][0-9]+$",$this->_current_token)) 
        {
        $result = $this->_current_token;
        $this->_advance();
        return($result);
        }
    elseif (is_numeric($this->_current_token))
        {
        $result = $this->_create_tree($this->_current_token, '', '');
        $this->_advance();
        return($result);
        }
    // if it's a function call
    elseif (eregi("^[A-Z0-9À-Ü\.]+$",$this->_current_token))
        {
        $result = $this->_func();
        return($result);
        }
    die("Sintactic error: ".$this->_current_token.", lookahead: ".
        $this->_lookahead.", current char: ".$this->_current_char);
    }

/**
* It parses a function call. It assumes the following rule:
* Func -> ( Expr [,Expr]* )
*
*/
  function _func()
    {
    $num_args = 0; // number of arguments received
    $function = $this->_current_token;
    $this->_advance();
    $this->_advance();         // eat the "("
    while($this->_current_token != ')')
        {
        if($num_args > 0)
            {
            if($this->_current_token == COMA) {
                $this->_advance();  // eat the ","
                }
            else {
                die("Sintactic error: coma expected $num_args");
                }
            $result = $this->_create_tree('arg', $result, $this->_expression());
            }
        else {
            $result = $this->_create_tree('arg', '', $this->_expression());
            }
        $num_args++;
        }
    $args = $this->_functions[$function][1];
    // If fixed number of args eg. TIME($i,$j,$k). Check that the number of args is valid.
    if (($args >= 0) and ($args != $num_args))
        {
        die("Incorrect number of arguments in function $function() ");
        }

    $result = $this->_create_tree($function, $result, '');
    $this->_advance();         // eat the ")"
    return($result);
    }

/**
* Creates a tree. In fact an array which may have one or two arrays (sub-trees)
* as elements.
*
* @param mixed $value The value of this node.
* @param mixed $left  The left array (sub-tree) or a final node.
* @param mixed $right The right array (sub-tree) or a final node.
*/
  function _create_tree($value, $left, $right)
    {
    return array('value' => $value, 'left' => $left, 'right' => $right);
    }

/**
* Builds a string containing the tree in reverse polish notation (What you 
* would use in a HP calculator stack).
* The following tree:
* 
*    +
*   / \
*  2   3
*
* produces: "23+"
*
* The following tree:
*
*    +
*   / \
*  3   *
*     / \
*    6   A1
*
* produces: "36A1*+"
*
* In fact all operands, functions, references, etc... are written as ptg's
*
* @access public
* @param array $tree The optional tree to convert.
*/
  function to_reverse_polish($tree = array())
    {
    $polish = ""; // the string we are going to return
    if (empty($tree)) // If it's the first call use _parse_tree
        {
        $tree = $this->_parse_tree;
        }
    if (is_array($tree['left']))
        {
        $polish .= $this->to_reverse_polish($tree['left']);
        }
    elseif($tree['left'] != '') // It's a final node
        {
        $polish .= $this->_convert($tree['left']); //$tree['left'];
        }
    if (is_array($tree['right']))
        {
        $polish .= $this->to_reverse_polish($tree['right']);
        }
    elseif($tree['right'] != '') // It's a final node
        {
        $polish .= $this->_convert($tree['right']);
        }
    $polish .= $this->_convert($tree['value']);
    return $polish;
    }
  }

/**
* Class for generating Excel Spreadsheets
*
* @author Xavier Noguer <xnoguer@rezebra.com>
* @package Spreadsheet_WriteExcel
*/

class Worksheet extends BIFFwriter
{

    /**
    * Constructor
    *
    * @param string  $name         The name of the new worksheet
    * @param integer $index        The index of the new worksheet
    * @param mixed   &$activesheet The current activesheet of the workbook we belong to
    * @param mixed   &$firstsheet  The first worksheet in the workbook we belong to 
    * @param mixed   &$url_format  The default format for hyperlinks
    * @param mixed   &$parser      The formula parser created for the Workbook
    */
    function Worksheet($name,$index,&$activesheet,&$firstsheet,&$url_format,&$parser)
    {
        $this->BIFFwriter();     // It needs to call its parent's constructor explicitly
        $rowmax                = 65536; // 16384 in Excel 5
        $colmax                = 256;
        $strmax                = 255;
    
        $this->name            = $name;
        $this->index           = $index;
        $this->activesheet     = &$activesheet;
        $this->firstsheet      = &$firstsheet;
        $this->_url_format     = $url_format;
        $this->_parser         = &$parser;
    
        $this->ext_sheets      = array();
        $this->_using_tmpfile  = 1;
        $this->_filehandle     = "";
        $this->fileclosed      = 0;
        $this->offset          = 0;
        $this->xls_rowmax      = $rowmax;
        $this->xls_colmax      = $colmax;
        $this->xls_strmax      = $strmax;
        $this->dim_rowmin      = $rowmax +1;
        $this->dim_rowmax      = 0;
        $this->dim_colmin      = $colmax +1;
        $this->dim_colmax      = 0;
        $this->colinfo         = array();
        $this->_selection      = array(0,0,0,0);
        $this->_panes          = array();
        $this->_active_pane    = 3;
        $this->_frozen         = 0;
        $this->selected        = 0;
    
        $this->_paper_size      = 0x0;
        $this->_orientation     = 0x1;
        $this->_header          = '';
        $this->_footer          = '';
        $this->_hcenter         = 0;
        $this->_vcenter         = 0;
        $this->_margin_head     = 0.50;
        $this->_margin_foot     = 0.50;
        $this->_margin_left     = 0.75;
        $this->_margin_right    = 0.75;
        $this->_margin_top      = 1.00;
        $this->_margin_bottom   = 1.00;
    
        $this->_title_rowmin    = NULL;
        $this->_title_rowmax    = NULL;
        $this->_title_colmin    = NULL;
        $this->_title_colmax    = NULL;
        $this->_print_rowmin    = NULL;
        $this->_print_rowmax    = NULL;
        $this->_print_colmin    = NULL;
        $this->_print_colmax    = NULL;
    
        $this->_print_gridlines = 1;
        $this->_print_headers   = 0;
    
        $this->_fit_page        = 0;
        $this->_fit_width       = 0;
        $this->_fit_height      = 0;
    
        $this->_hbreaks         = array();
        $this->_vbreaks         = array();
    
        $this->_protect         = 0;
        $this->_password        = NULL;
    
        $this->col_sizes        = array();
        $this->row_sizes        = array();
    
        $this->_zoom            = 100;
        $this->_print_scale     = 100;
        $this->_rtl				= 0;	// Added by Joe Hunt 2009-03-05 for arabic languages
    
        $this->_initialize();
    }
    
    /**
    * Open a tmp file to store the majority of the Worksheet data. If this fails,
    * for example due to write permissions, store the data in memory. This can be
    * slow for large files.
    */
    function _initialize()
    {
        // Open tmp file for storing Worksheet data
        $fh = tmpfile();
        if ( $fh) {
            // Store filehandle
            $this->_filehandle = $fh;
        }
        else {
            // If tmpfile() fails store data in memory
            $this->_using_tmpfile = 0;
        }
    }
    
    /**
    * Add data to the beginning of the workbook (note the reverse order)
    * and to the end of the workbook.
    *
    * @access public 
    * @see Workbook::store_workbook()
    * @param array $sheetnames The array of sheetnames from the Workbook this 
    *                          worksheet belongs to
    */
    function close($sheetnames)
    {
        $num_sheets = count($sheetnames);

        /***********************************************
        * Prepend in reverse order!!
        */
    
        // Prepend the sheet dimensions
        $this->_store_dimensions();
    
        // Prepend the sheet password
        $this->_store_password();
    
        // Prepend the sheet protection
        $this->_store_protect();
    
        // Prepend the page setup
        $this->_store_setup();
    
        // Prepend the bottom margin
        $this->_store_margin_bottom();
    
        // Prepend the top margin
        $this->_store_margin_top();
    
        // Prepend the right margin
        $this->_store_margin_right();
    
        // Prepend the left margin
        $this->_store_margin_left();
    
        // Prepend the page vertical centering
        $this->store_vcenter();
    
        // Prepend the page horizontal centering
        $this->store_hcenter();
    
        // Prepend the page footer
        $this->store_footer();
    
        // Prepend the page header
        $this->store_header();
    
        // Prepend the vertical page breaks
        $this->_store_vbreak();
    
        // Prepend the horizontal page breaks
        $this->_store_hbreak();
    
        // Prepend WSBOOL
        $this->_store_wsbool();
    
        // Prepend GRIDSET
        $this->_store_gridset();
    
        // Prepend PRINTGRIDLINES
        $this->_store_print_gridlines();
    
        // Prepend PRINTHEADERS
        $this->_store_print_headers();
    
        // Prepend EXTERNSHEET references
        for ($i = $num_sheets; $i > 0; $i--) {
            $sheetname = $sheetnames[$i-1];
            $this->_store_externsheet($sheetname);
        }
    
        // Prepend the EXTERNCOUNT of external references.
        $this->_store_externcount($num_sheets);
    
        // Prepend the COLINFO records if they exist
        if (!empty($this->colinfo)){
            for($i=0; $i < count($this->colinfo); $i++)
            {
                $this->_store_colinfo($this->colinfo[$i]);
            }
            $this->_store_defcol();
        }
    
        // Prepend the BOF record
        $this->_store_bof(0x0010);
    
        /*
        * End of prepend. Read upwards from here.
        ***********************************************/
    
        // Append
        $this->_store_window2();
        $this->_store_zoom();
        if(!empty($this->_panes))
          $this->_store_panes($this->_panes);
        $this->_store_selection($this->_selection);
        $this->_store_eof();
    }
    
    /**
    * Retrieve the worksheet name. This is usefull when creating worksheets
    * without a name.
    *
    * @access public
    * @return string The worksheet's name
    */
    function get_name()
    {
        return($this->name);
    }
    
    /**
    * Retrieves data from memory in one chunk, or from disk in $buffer
    * sized chunks.
    *
    * @return string The data
    */
    function get_data()
    {
        $buffer = 4096;
    
        // Return data stored in memory
        if (isset($this->_data)) {
            $tmp   = $this->_data;
            unset($this->_data);
            $fh    = $this->_filehandle;
            if ($this->_using_tmpfile) {
                fseek($fh, 0);
            }
            return($tmp);
        }
        // Return data stored on disk
        if ($this->_using_tmpfile) {
            if ($tmp = fread($this->_filehandle, $buffer)) {
                return($tmp);
            }
        }
    
        // No data to return
        return('');
    }
    
    /**
    * Set this worksheet as a selected worksheet, i.e. the worksheet has its tab
    * highlighted.
    *
    * @access public
    */
    function select()
    {
        $this->selected = 1;
    }
    
    /**
    * Set this worksheet as the active worksheet, i.e. the worksheet that is
    * displayed when the workbook is opened. Also set it as selected.
    *
    * @access public
    */
    function activate()
    {
        $this->selected = 1;
        $this->activesheet =& $this->index;
    }
    
    /**
    * Set this worksheet as the first visible sheet. This is necessary
    * when there are a large number of worksheets and the activated
    * worksheet is not visible on the screen.
    *
    * @access public
    */
    function set_first_sheet()
    {
        $this->firstsheet = $this->index;
    }
    
    /**
    * Set the worksheet protection flag to prevent accidental modification and to
    * hide formulas if the locked and hidden format properties have been set.
    *
    * @access public
    * @param string $password The password to use for protecting the sheet.
    */
    function protect($password)
    {
        $this->_protect   = 1;
        $this->_password  = $this->_encode_password($password);
    }
    
    /**
    * Set the width of a single column or a range of columns.
    *
    * @access public
    * @see _store_colinfo()
    * @param integer $firstcol first column on the range
    * @param integer $lastcol  last column on the range
    * @param integer $width    width to set
    * @param mixed   $format   The optional XF format to apply to the columns
    * @param integer $hidden   The optional hidden atribute
    */
    function set_column($firstcol, $lastcol, $width, $format = null, $hidden = 0)
    {
        $this->colinfo[] = array($firstcol, $lastcol, $width, $format, $hidden);

        // Set width to zero if column is hidden
        $width = ($hidden) ? 0 : $width;
    
        for($col = $firstcol; $col <= $lastcol; $col++) {
            $this->col_sizes[$col] = $width;
        }
    }
    
    /**
    * Set which cell or cells are selected in a worksheet
    *
    * @access public
    * @param integer $first_row    first row in the selected quadrant
    * @param integer $first_column first column in the selected quadrant
    * @param integer $last_row     last row in the selected quadrant
    * @param integer $last_column  last column in the selected quadrant
    * @see _store_selection()
    */
    function set_selection($first_row,$first_column,$last_row,$last_column)
    {
        $this->_selection = array($first_row,$first_column,$last_row,$last_column);
    }
    
    /**
    * Set panes and mark them as frozen.
    *
    * @access public
    * @param array $panes This is the only parameter received and is composed of the following:
    *                     0 => Vertical split position,
    *                     1 => Horizontal split position
    *                     2 => Top row visible
    *                     3 => Leftmost column visible
    *                     4 => Active pane
    */
    function freeze_panes($panes)
    {
        $this->_frozen = 1;
        $this->_panes  = $panes;
    }
    
    /**
    * Set panes and mark them as unfrozen.
    *
    * @access public
    * @param array $panes This is the only parameter received and is composed of the following:
    *                     0 => Vertical split position,
    *                     1 => Horizontal split position
    *                     2 => Top row visible
    *                     3 => Leftmost column visible
    *                     4 => Active pane
    */
    function thaw_panes($panes)
    {
        $this->_frozen = 0;
        $this->_panes  = $panes;
    }
    
    /**
    * Set the page orientation as portrait.
    *
    * @access public
    */
    function set_portrait()
    {
        $this->_orientation = 1;
    }
    
    /**
    * Set the page orientation as landscape.
    *
    * @access public
    */
    function set_landscape()
    {
        $this->_orientation = 0;
    }
    
    /**
    * Set the paper type. Ex. 1 = US Letter, 9 = A4
    *
    * @access public
    * @param integer $size The type of paper size to use
    */
    function set_paper($size = 0)
    {
        $this->_paper_size = $size;
    }
    
    
    /**
    * Set the page header caption and optional margin.
    *
    * @access public
    * @param string $string The header text
    * @param float  $margin optional head margin in inches.
    */
    function set_header($string,$margin = 0.50)
    {
        if (strlen($string) >= 255) {
            //carp 'Header string must be less than 255 characters';
            return;
        }
        $this->_header      = $string;
        $this->_margin_head = $margin;
    }
    
    /**
    * Set the page footer caption and optional margin.
    *
    * @access public
    * @param string $string The footer text
    * @param float  $margin optional foot margin in inches.
    */
    function set_footer($string,$margin = 0.50)
    {
        if (strlen($string) >= 255) {
            //carp 'Footer string must be less than 255 characters';
            return;
        }
        $this->_footer      = $string;
        $this->_margin_foot = $margin;
    }
    
    /**
    * Center the page horinzontally.
    *
    * @access public
    * @param integer $center the optional value for centering. Defaults to 1 (center).
    */
    function center_horizontally($center = 1)
    {
        $this->_hcenter = $center;
    }
    
    /**
    * Center the page horinzontally.
    *
    * @access public
    * @param integer $center the optional value for centering. Defaults to 1 (center).
    */
    function center_vertically($center = 1)
    {
        $this->_vcenter = $center;
    }
    
    /**
    * Set all the page margins to the same value in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margins($margin)
    {
        $this->set_margin_left($margin);
        $this->set_margin_right($margin);
        $this->set_margin_top($margin);
        $this->set_margin_bottom($margin);
    }
    
    /**
    * Set the left and right margins to the same value in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margins_LR($margin)
    {
        $this->set_margin_left($margin);
        $this->set_margin_right($margin);
    }
    
    /**
    * Set the top and bottom margins to the same value in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margins_TB($margin)
    {
        $this->set_margin_top($margin);
        $this->set_margin_bottom($margin);
    }
    
    /**
    * Set the left margin in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margin_left($margin = 0.75)
    {
        $this->_margin_left = $margin;
    }
    
    /**
    * Set the right margin in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margin_right($margin = 0.75)
    {
        $this->_margin_right = $margin;
    }
    
    /**
    * Set the top margin in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margin_top($margin = 1.00)
    {
        $this->_margin_top = $margin;
    }
    
    /**
    * Set the bottom margin in inches.
    *
    * @access public
    * @param float $margin The margin to set in inches
    */
    function set_margin_bottom($margin = 1.00)
    {
        $this->_margin_bottom = $margin;
    }
    
    /**
    * Set the rows to repeat at the top of each printed page. See also the
    * _store_name_xxxx() methods in Workbook.php
    *
    * @access public
    * @param integer $first_row First row to repeat
    * @param integer $last_row  Last row to repeat. Optional.
    */
    function repeat_rows($first_row, $last_row = NULL)
    {
        $this->_title_rowmin  = $first_row;
        if(isset($last_row)) { //Second row is optional
            $this->_title_rowmax  = $last_row;
        }
        else {
            $this->_title_rowmax  = $first_row;
        }
    }
    
    /**
    * Set the columns to repeat at the left hand side of each printed page.
    * See also the _store_names() methods in Workbook.php
    *
    * @access public
    * @param integer $first_col First column to repeat
    * @param integer $last_col  Last column to repeat. Optional.
    */
    function repeat_columns($first_col, $last_col = NULL)
    {
        $this->_title_colmin  = $first_col;
        if(isset($last_col)) { // Second col is optional
            $this->_title_colmax  = $last_col;
        }
        else {
            $this->_title_colmax  = $first_col;
        }
    }
    
    /**
    * Set the area of each worksheet that will be printed.
    *
    * @access public
    * @see Workbook::_store_names()
    * @param integer $first_row First row of the area to print
    * @param integer $first_col First column of the area to print
    * @param integer $last_row  Last row of the area to print
    * @param integer $last_col  Last column of the area to print
    */
    function print_area($first_row, $first_col, $last_row, $last_col)
    {
        $this->_print_rowmin  = $first_row;
        $this->_print_colmin  = $first_col;
        $this->_print_rowmax  = $last_row;
        $this->_print_colmax  = $last_col;
    }
    
    
    /**
    * Set the option to hide gridlines on the printed page. 
    *
    * @access public
    * @see _store_print_gridlines(), _store_gridset()
    */
    function hide_gridlines()
    {
        $this->_print_gridlines = 0;
    }
    
    /**
    * Set the option to print the row and column headers on the printed page.
    * See also the _store_print_headers() method below.
    *
    * @access public
    * @see _store_print_headers()
    * @param integer $print Whether to print the headers or not. Defaults to 1 (print).
    */
    function print_row_col_headers($print = 1)
    {
        $this->_print_headers = $print;
    }
    
    /**
    * Store the vertical and horizontal number of pages that will define the
    * maximum area printed. It doesn't seem to work with OpenOffice.
    *
    * @access public
    * @param  integer $width  Maximun width of printed area in pages
    * @param  integer $heigth Maximun heigth of printed area in pages
    * @see set_print_scale()
    */
    function fit_to_pages($width, $height)
    {
        $this->_fit_page      = 1;
        $this->_fit_width     = $width;
        $this->_fit_height    = $height;
    }
    
    /**
    * Store the horizontal page breaks on a worksheet (for printing).
    * The breaks represent the row after which the break is inserted.
    *
    * @access public
    * @param array $breaks Array containing the horizontal page breaks
    */
    function set_h_pagebreaks($breaks)
    {
        foreach($breaks as $break) {
            array_push($this->_hbreaks,$break);
        }
    }
    
    /**
    * Store the vertical page breaks on a worksheet (for printing).
    * The breaks represent the column after which the break is inserted.
    *
    * @access public
    * @param array $breaks Array containing the vertical page breaks
    */
    function set_v_pagebreaks($breaks)
    {
        foreach($breaks as $break) {
            array_push($this->_vbreaks,$break);
        }
    }
    
    
    /**
    * Set the worksheet zoom factor.
    *
    * @access public
    * @param integer $scale The zoom factor
    */
    function set_zoom($scale = 100)
    {
        // Confine the scale to Excel's range
        if ($scale < 10 or $scale > 400) {
            //carp "Zoom factor $scale outside range: 10 <= zoom <= 400";
            $scale = 100;
        }
    
        $this->_zoom = floor($scale);
    }
    
    /**
    * Set the scale factor for the printed page. 
    * It turns off the "fit to page" option
    *
    * @access public
    * @param integer $scale The optional scale factor. Defaults to 100
    */
    function set_print_scale($scale = 100)
    {
        // Confine the scale to Excel's range
        if ($scale < 10 or $scale > 400)
        {
            // REPLACE THIS FOR A WARNING
            die("Print scale $scale outside range: 10 <= zoom <= 400");
            $scale = 100;
        }
    
        // Turn off "fit to page" option
        $this->_fit_page    = 0;
    
        $this->_print_scale = floor($scale);
    }
    
    /** added 2009-03-05 by Joe Hunt, FA for arabic languages */
    function set_rtl()
    {
    	$this->_rtl = 1;
    }	
    /**
    * Map to the appropriate write method acording to the token recieved.
    *
    * @access public
    * @param integer $row    The row of the cell we are writing to
    * @param integer $col    The column of the cell we are writing to
    * @param mixed   $token  What we are writing
    * @param mixed   $format The optional format to apply to the cell
    */
    function write($row, $col, $token, $format = null)
    {
        // Check for a cell reference in A1 notation and substitute row and column
        /*if ($_[0] =~ /^\D/) {
            @_ = $this->_substitute_cellref(@_);
    }*/
    
        /*
        # Match an array ref.
        if (ref $token eq "ARRAY") {
            return $this->write_row(@_);
    }*/
    
        // Match number
        if (preg_match("/^([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?$/",$token)) {
            return $this->write_number($row,$col,$token,$format);
        }
        // Match http or ftp URL
        elseif (preg_match("/^[fh]tt?p:\/\//",$token)) {
            return $this->write_url($row, $col, $token, $format);
        }
        // Match mailto:
        elseif (preg_match("/^mailto:/",$token)) {
            return $this->write_url($row, $col, $token, $format);
        }
        // Match internal or external sheet link
        elseif (preg_match("/^(?:in|ex)ternal:/",$token)) {
            return $this->write_url($row, $col, $token, $format);
        }
        // Match formula
        elseif (preg_match("/^=/",$token)) {
            return $this->write_formula($row, $col, $token, $format);
        }
        // Match formula
        elseif (preg_match("/^@/",$token)) {
            return $this->write_formula($row, $col, $token, $format);
        }
        // Match blank
        elseif ($token == '') {
            return $this->write_blank($row,$col,$format);
        }
        // Default: match string
        else {
            return $this->write_string($row,$col,$token,$format);
        }
    }
 
    /**
    * Returns an index to the XF record in the workbook
    *
    * @param mixed $format The optional XF format
    * @return integer The XF record index
    */
    //function _XF(&$format)
    function _XF($format=null)
    {
        if($format != null)
        {
            return($format->get_xf_index());
        }
        else
        {
            return(0x0F);
        }
    }
    
    
    /******************************************************************************
    *******************************************************************************
    *
    * Internal methods
    */
    
    
    /**
    * Store Worksheet data in memory using the parent's class append() or to a
    * temporary file, the default.
    *
    * @param string $data The binary data to append
    */
    function _append($data)
    {
        if ($this->_using_tmpfile)
        {
            // Add CONTINUE records if necessary
            if (strlen($data) > $this->_limit) {
                $data = $this->_add_continue($data);
            }
            fwrite($this->_filehandle,$data);
            $this->_datasize += strlen($data);
        }
        else {
            parent::_append($data);
        }
    }
    
    /**
    * Substitute an Excel cell reference in A1 notation for  zero based row and
    * column values in an argument list.
    *
    * Ex: ("A4", "Hello") is converted to (3, 0, "Hello").
    *
    * @param string $cell The cell reference. Or range of cells.
    * @return array
    */
    function _substitute_cellref($cell)
    {
        $cell = strtoupper($cell);
    
        // Convert a column range: 'A:A' or 'B:G'
        if (preg_match("/([A-I]?[A-Z]):([A-I]?[A-Z])/",$cell,$match)) {
            list($no_use, $col1) =  $this->_cell_to_rowcol($match[1] .'1'); // Add a dummy row
            list($no_use, $col2) =  $this->_cell_to_rowcol($match[2] .'1'); // Add a dummy row
            return(array($col1, $col2));
        }
    
        // Convert a cell range: 'A1:B7'
        if (preg_match("/\$?([A-I]?[A-Z]\$?\d+):\$?([A-I]?[A-Z]\$?\d+)/",$cell,$match)) {
            list($row1, $col1) =  $this->_cell_to_rowcol($match[1]);
            list($row2, $col2) =  $this->_cell_to_rowcol($match[2]);
            return(array($row1, $col1, $row2, $col2));
        }
    
        // Convert a cell reference: 'A1' or 'AD2000'
        if (preg_match("/\$?([A-I]?[A-Z]\$?\d+)/",$cell)) {
            list($row1, $col1) =  $this->_cell_to_rowcol($match[1]);
            return(array($row1, $col1));
        }
    
        die("Unknown cell reference $cell ");
    }
    
    /**
    * Convert an Excel cell reference in A1 notation to a zero based row and column
    * reference; converts C1 to (0, 2).
    *
    * @param string $cell The cell reference.
    * @return array containing (row, column)
    */
    function _cell_to_rowcol($cell)
    {
        preg_match("/\$?([A-I]?[A-Z])\$?(\d+)/",$cell,$match);
        $col     = $match[1];
        $row     = $match[2];
    
        // Convert base26 column string to number
        $chars = split('', $col);
        $expn  = 0;
        $col   = 0;
    
        while ($chars) {
            $char = array_pop($chars);        // LS char first
            $col += (ord($char) -ord('A') +1) * pow(26,$expn);
            $expn++;
        }
    
        // Convert 1-index to zero-index
        $row--;
        $col--;
    
        return(array($row, $col));
    }
    
    /**
    * Based on the algorithm provided by Daniel Rentz of OpenOffice.
    *
    * @param string $plaintext The password to be encoded in plaintext.
    * @return string The encoded password
    */
    function _encode_password($plaintext)
    {
        $password = 0x0000;
        $i        = 1;       // char position
 
        // split the plain text password in its component characters
        $chars = preg_split('//', $plaintext, -1, PREG_SPLIT_NO_EMPTY);
        foreach($chars as $char)
        {
            $value     = ord($char) << $i;   // shifted ASCII value 
            $bit_16    = $value & 0x8000;    // the bit 16
            $bit_16  >>= 15;                 // 0x0000 or 0x0001
            //$bit_17    = $value & 0x00010000;
            //$bit_17  >>= 15;
            $value    &= 0x7fff;             // first 15 bits
            $password ^= ($value | $bit_16);
            //$password ^= ($value | $bit_16 | $bit_17);
            $i++;
        }
    
        $password ^= strlen($plaintext);
        $password ^= 0xCE4B;

        return($password);
    }
    
    /******************************************************************************
    *******************************************************************************
    *
    * BIFF RECORDS
    */
    
    
    /**
    * Write a double to the specified row and column (zero indexed).
    * An integer can be written as a double. Excel will display an
    * integer. $format is optional.
    *
    * Returns  0 : normal termination
    *         -2 : row or column out of range
    *
    * @access public
    * @param integer $row    Zero indexed row
    * @param integer $col    Zero indexed column
    * @param float   $num    The number to write
    * @param mixed   $format The optional XF format
    */
    function write_number($row, $col, $num, $format = null)
    {
        $record    = 0x0203;                 // Record identifier
        $length    = 0x000E;                 // Number of bytes to follow
        $xf        = $this->_XF($format);    // The cell format
    
        // Check that row and col are valid and store max and min values
        if ($row >= $this->xls_rowmax)
        {
            return(-2);
        }
        if ($col >= $this->xls_colmax)
        {
            return(-2);
        }
        if ($row <  $this->dim_rowmin) 
        {
            $this->dim_rowmin = $row;
        }
        if ($row >  $this->dim_rowmax) 
        {
            $this->dim_rowmax = $row;
        }
        if ($col <  $this->dim_colmin) 
        {
            $this->dim_colmin = $col;
        }
        if ($col >  $this->dim_colmax) 
        {
            $this->dim_colmax = $col;
        }
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("vvv", $row, $col, $xf);
        $xl_double = pack("d",   $num);
        if ($this->_byte_order) // if it's Big Endian
        {
            $xl_double = strrev($xl_double);
        }
    
        $this->_append($header.$data.$xl_double);
        return(0);
    }
    
    /**
    * Write a string to the specified row and column (zero indexed).
    * NOTE: there is an Excel 5 defined limit of 255 characters.
    * $format is optional.
    * Returns  0 : normal termination
    *         -1 : insufficient number of arguments
    *         -2 : row or column out of range
    *         -3 : long string truncated to 255 chars
    *
    * @access public
    * @param integer $row    Zero indexed row
    * @param integer $col    Zero indexed column
    * @param string  $str    The string to write
    * @param mixed   $format The XF format for the cell
    */
    function write_string($row, $col, $str, $format = null)
    {
        $strlen    = strlen($str);
        $record    = 0x0204;                   // Record identifier
        $length    = 0x0008 + $strlen;         // Bytes to follow
        $xf        = $this->_XF($format);      // The cell format
        
        $str_error = 0;
    
        // Check that row and col are valid and store max and min values
        if ($row >= $this->xls_rowmax) 
        {
            return(-2);
        }
        if ($col >= $this->xls_colmax) 
        {
            return(-2);
        }
        if ($row <  $this->dim_rowmin) 
        {
            $this->dim_rowmin = $row;
        }
        if ($row >  $this->dim_rowmax) 
        {
            $this->dim_rowmax = $row;
        }
        if ($col <  $this->dim_colmin) 
        {
            $this->dim_colmin = $col;
        }
        if ($col >  $this->dim_colmax) 
        {
            $this->dim_colmax = $col;
        }
    
        if ($strlen > $this->xls_strmax)  // LABEL must be < 255 chars
        {
            $str       = substr($str, 0, $this->xls_strmax);
            $length    = 0x0008 + $this->xls_strmax;
            $strlen    = $this->xls_strmax;
            $str_error = -3;
        }
    
        $header    = pack("vv",   $record, $length);
        $data      = pack("vvvv", $row, $col, $xf, $strlen);
        $this->_append($header.$data.$str);
        return($str_error);
    }
 
    /**
    * Writes a note associated with the cell given by the row and column.
    * NOTE records don't have a length limit.
    *
    * @access public
    * @param integer $row    Zero indexed row
    * @param integer $col    Zero indexed column
    * @param string  $note   The note to write
    */
    function write_note($row, $col, $note)
    {
        $note_length    = strlen($note);
        $record         = 0x001C;                // Record identifier
        $max_length     = 2048;                  // Maximun length for a NOTE record
        //$length      = 0x0006 + $note_length;    // Bytes to follow

        // Check that row and col are valid and store max and min values
        if ($row >= $this->xls_rowmax) 
        {
            return(-2);
        }
        if ($col >= $this->xls_colmax) 
        {
            return(-2);
        }
        if ($row <  $this->dim_rowmin) 
        {
            $this->dim_rowmin = $row;
        }
        if ($row >  $this->dim_rowmax) 
        {
            $this->dim_rowmax = $row;
        }
        if ($col <  $this->dim_colmin) 
        {
            $this->dim_colmin = $col;
        }
        if ($col >  $this->dim_colmax) 
        {
            $this->dim_colmax = $col;
        }
 
        // Length for this record is no more than 2048 + 6
        $length    = 0x0006 + min($note_length, 2048);
        $header    = pack("vv",   $record, $length);
        $data      = pack("vvv", $row, $col, $note_length);
        $this->_append($header.$data.substr($note, 0, 2048));

        for($i = $max_length; $i < $note_length; $i += $max_length)
        {
            $chunk  = substr($note, $i, $max_length);
            $length = 0x0006 + strlen($chunk);
            $header = pack("vv",   $record, $length);
            $data   = pack("vvv", -1, 0, strlen($chunk));
            $this->_append($header.$data.$chunk);
        }
        return(0);
    }
 
    /**
    * Write a blank cell to the specified row and column (zero indexed).
    * A blank cell is used to specify formatting without adding a string
    * or a number.
    *
    * A blank cell without a format serves no purpose. Therefore, we don't write
    * a BLANK record unless a format is specified. This is mainly an optimisation
    * for the write_row() and write_col() methods.
    *
    * Returns  0 : normal termination (including no format)
    *         -1 : insufficient number of arguments
    *         -2 : row or column out of range
    *
    * @access public
    * @param integer $row    Zero indexed row
    * @param integer $col    Zero indexed column
    * @param mixed   $format The XF format
    */
    function write_blank($row, $col, $format = null)
    {
        // Don't write a blank cell unless it has a format
        if ($format == null)
        {
            return(0);
        }
    
        $record    = 0x0201;                 // Record identifier
        $length    = 0x0006;                 // Number of bytes to follow
        $xf        = $this->_XF($format);    // The cell format
    
        // Check that row and col are valid and store max and min values
        if ($row >= $this->xls_rowmax) 
        {
            return(-2);
        }
        if ($col >= $this->xls_colmax) 
        {
            return(-2);
        }
        if ($row <  $this->dim_rowmin) 
        {
            $this->dim_rowmin = $row;
        }
        if ($row >  $this->dim_rowmax) 
        {
            $this->dim_rowmax = $row;
        }
        if ($col <  $this->dim_colmin) 
        {
            $this->dim_colmin = $col;
        }
        if ($col >  $this->dim_colmax) 
        {
            $this->dim_colmax = $col;
        }
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("vvv", $row, $col, $xf);
        $this->_append($header.$data);
        return 0;
    }
 
    /**
    * Write a formula to the specified row and column (zero indexed).
    * The textual representation of the formula is passed to the parser in
    * Parser.php which returns a packed binary string.
    *
    * Returns  0 : normal termination
    *         -2 : row or column out of range
    *
    * @access public
    * @param integer $row     Zero indexed row
    * @param integer $col     Zero indexed column
    * @param string  $formula The formula text string
    * @param mixed   $format  The optional XF format
    */
    function write_formula($row, $col, $formula, $format = null)
    {
        $record    = 0x0006;     // Record identifier
    
        // Excel normally stores the last calculated value of the formula in $num.
        // Clearly we are not in a position to calculate this a priori. Instead
        // we set $num to zero and set the option flags in $grbit to ensure
        // automatic calculation of the formula when the file is opened.
        //
        $xf        = $this->_XF($format); // The cell format
        $num       = 0x00;                // Current value of formula
        $grbit     = 0x03;                // Option flags
        $chn       = 0x0000;              // Must be zero
    
    
        // Check that row and col are valid and store max and min values
        if ($row >= $this->xls_rowmax)
        {
            return(-2);
        }
        if ($col >= $this->xls_colmax)
        {
            return(-2);
        }
        if ($row <  $this->dim_rowmin) 
        {
            $this->dim_rowmin = $row;
        }
        if ($row >  $this->dim_rowmax) 
        {
            $this->dim_rowmax = $row;
        }
        if ($col <  $this->dim_colmin) 
        {
            $this->dim_colmin = $col;
        }
        if ($col >  $this->dim_colmax) 
        {
            $this->dim_colmax = $col;
        }
    
        // Strip the '=' or '@' sign at the beginning of the formula string
        if (ereg("^=",$formula)) {
            $formula = preg_replace("/(^=)/","",$formula);
        }
        elseif(ereg("^@",$formula)) {
            $formula = preg_replace("/(^@)/","",$formula);
        }
        else {
            die("Unrecognised character for formula");
        }
    
        // Parse the formula using the parser in Parser.php
        //$tree      = new Parser($this->_byte_order);
        $this->_parser->parse($formula);
        //$tree->parse($formula);
        $formula = $this->_parser->to_reverse_polish();
    
        $formlen    = strlen($formula);    // Length of the binary string
        $length     = 0x16 + $formlen;     // Length of the record data
    
        $header    = pack("vv",      $record, $length);
        $data      = pack("vvvdvVv", $row, $col, $xf, $num,
                                     $grbit, $chn, $formlen);
    
        $this->_append($header.$data.$formula);
        return 0;
    }
    
    /**
    * Write a hyperlink. This is comprised of two elements: the visible label and
    * the invisible link. The visible label is the same as the link unless an
    * alternative string is specified. The label is written using the
    * write_string() method. Therefore the 255 characters string limit applies.
    * $string and $format are optional and their order is interchangeable.
    *
    * The hyperlink can be to a http, ftp, mail, internal sheet, or external
    * directory url.
    *
    * Returns  0 : normal termination
    *         -1 : insufficient number of arguments
    *         -2 : row or column out of range
    *         -3 : long string truncated to 255 chars
    *
    * @access public
    * @param integer $row    Row
    * @param integer $col    Column
    * @param string  $url    URL string
    * @param string  $string Alternative label
    * @param mixed   $format The cell format
    */
    function write_url($row, $col, $url, $string = '', $format = null)
    {
        // Add start row and col to arg list
        return($this->_write_url_range($row, $col, $row, $col, $url, $string, $format));
    }
    
    /**
    * This is the more general form of write_url(). It allows a hyperlink to be
    * written to a range of cells. This function also decides the type of hyperlink
    * to be written. These are either, Web (http, ftp, mailto), Internal
    * (Sheet1!A1) or external ('c:\temp\foo.xls#Sheet1!A1').
    *
    * See also write_url() above for a general description and return values.
    *
    * @param integer $row1   Start row
    * @param integer $col1   Start column
    * @param integer $row2   End row
    * @param integer $col2   End column
    * @param string  $url    URL string
    * @param string  $string Alternative label
    * @param mixed   $format The cell format
    */
    
    function _write_url_range($row1, $col1, $row2, $col2, $url, $string = '', $format = null)
    {
        // Check for internal/external sheet links or default to web link
        if (preg_match('[^internal:]', $url)) {
            return($this->_write_url_internal($row1, $col1, $row2, $col2, $url, $string, $format));
        }
        if (preg_match('[^external:]', $url)) {
            return($this->_write_url_external($row1, $col1, $row2, $col2, $url, $string, $format));
        }
        return($this->_write_url_web($row1, $col1, $row2, $col2, $url, $string, $format));
    }
    
    
    /**
    * Used to write http, ftp and mailto hyperlinks.
    * The link type ($options) is 0x03 is the same as absolute dir ref without
    * sheet. However it is differentiated by the $unknown2 data stream.
    *
    * @see write_url()
    * @param integer $row1   Start row
    * @param integer $col1   Start column
    * @param integer $row2   End row
    * @param integer $col2   End column
    * @param string  $url    URL string
    * @param string  $str    Alternative label
    * @param mixed   $format The cell format
    */
    function _write_url_web($row1, $col1, $row2, $col2, $url, $str, $format = null)
    {
        $record      = 0x01B8;                       // Record identifier
        $length      = 0x00000;                      // Bytes to follow
    
        if($format == null) {
            $format = $this->_url_format;
        }
    
        // Write the visible label using the write_string() method.
        if($str == '') {
            $str = $url;
        }
        $str_error = $this->write_string($row1, $col1, $str, $format);
        if ($str_error == -2) {
            return($str_error);
        }
    
        // Pack the undocumented parts of the hyperlink stream
        $unknown1    = pack("H*", "D0C9EA79F9BACE118C8200AA004BA90B02000000");
        $unknown2    = pack("H*", "E0C9EA79F9BACE118C8200AA004BA90B");
    
        // Pack the option flags
        $options     = pack("V", 0x03);
    
        // Convert URL to a null terminated wchar string
        $url         = join("\0", preg_split("''", $url, -1, PREG_SPLIT_NO_EMPTY));
        $url         = $url . "\0\0\0";
    
        // Pack the length of the URL
        $url_len     = pack("V", strlen($url));
    
        // Calculate the data length
        $length      = 0x34 + strlen($url);
    
        // Pack the header data
        $header      = pack("vv",   $record, $length);
        $data        = pack("vvvv", $row1, $row2, $col1, $col2);
    
        // Write the packed data
        $this->_append( $header. $data.
                        $unknown1. $options.
                        $unknown2. $url_len. $url);
        return($str_error);
    }
    
    /**
    * Used to write internal reference hyperlinks such as "Sheet1!A1".
    *
    * @see write_url()
    * @param integer $row1   Start row
    * @param integer $col1   Start column
    * @param integer $row2   End row
    * @param integer $col2   End column
    * @param string  $url    URL string
    * @param string  $str    Alternative label
    * @param mixed   $format The cell format
    */
    function _write_url_internal($row1, $col1, $row2, $col2, $url, $str, $format = null)
    {
        $record      = 0x01B8;                       // Record identifier
        $length      = 0x00000;                      // Bytes to follow
    
        if ($format == null) {
            $format = $this->_url_format;
        }
    
        // Strip URL type
        $url = preg_replace('s[^internal:]', '', $url);
    
        // Write the visible label
        if($str == '') {
            $str = $url;
        }
        $str_error = $this->write_string($row1, $col1, $str, $format);
        if ($str_error == -2) {
            return($str_error);
        }
    
        // Pack the undocumented parts of the hyperlink stream
        $unknown1    = pack("H*", "D0C9EA79F9BACE118C8200AA004BA90B02000000");
    
        // Pack the option flags
        $options     = pack("V", 0x08);
    
        // Convert the URL type and to a null terminated wchar string
        $url         = join("\0", preg_split("''", $url, -1, PREG_SPLIT_NO_EMPTY));
        $url         = $url . "\0\0\0";
    
        // Pack the length of the URL as chars (not wchars)
        $url_len     = pack("V", floor(strlen($url)/2));
    
        // Calculate the data length
        $length      = 0x24 + strlen($url);
    
        // Pack the header data
        $header      = pack("vv",   $record, $length);
        $data        = pack("vvvv", $row1, $row2, $col1, $col2);
    
        // Write the packed data
        $this->_append($header. $data.
                       $unknown1. $options.
                       $url_len. $url);
        return($str_error);
    }
    
    /**
    * Write links to external directory names such as 'c:\foo.xls',
    * c:\foo.xls#Sheet1!A1', '../../foo.xls'. and '../../foo.xls#Sheet1!A1'.
    *
    * Note: Excel writes some relative links with the $dir_long string. We ignore
    * these cases for the sake of simpler code.
    *
    * @see write_url()
    * @param integer $row1   Start row
    * @param integer $col1   Start column
    * @param integer $row2   End row
    * @param integer $col2   End column
    * @param string  $url    URL string
    * @param string  $str    Alternative label
    * @param mixed   $format The cell format
    */
    function _write_url_external($row1, $col1, $row2, $col2, $url, $str, $format = null)
    {
        // Network drives are different. We will handle them separately
        // MS/Novell network drives and shares start with \\
        if (preg_match('[^external:\\\\]', $url)) {
            return($this->_write_url_external_net($row1, $col1, $row2, $col2, $url, $str, $format));
        }
    
        $record      = 0x01B8;                       // Record identifier
        $length      = 0x00000;                      // Bytes to follow
    
        if ($format == null) {
            $format = $this->_url_format;
        }
    
        // Strip URL type and change Unix dir separator to Dos style (if needed)
        //
        $url = preg_replace('[^external:]', '', $url);
        $url = preg_replace('[/]', "\\", $url);
    
        // Write the visible label
        if ($str == '') {
            $str = preg_replace('[\#]', ' - ', $url);
        }
        $str_error = $this->write_string($row1, $col1, $str, $format);
        if ($str_error == -2) {
            return($str_error);
        }
    
        // Determine if the link is relative or absolute:
        //   relative if link contains no dir separator, "somefile.xls"
        //   relative if link starts with up-dir, "..\..\somefile.xls"
        //   otherwise, absolute
        
        $absolute    = 0x02; // Bit mask
        if (!preg_match('[\\]', $url)) {
            $absolute    = 0x00;
        }
        if (preg_match('[^\.\.\\]', $url)) {
            $absolute    = 0x00;
        }
    
        // Determine if the link contains a sheet reference and change some of the
        // parameters accordingly.
        // Split the dir name and sheet name (if it exists)
        list($dir_long , $sheet) = split('/\#/', $url);
        $link_type               = 0x01 | $absolute;
    
        if (isset($sheet)) {
            $link_type |= 0x08;
            $sheet_len  = pack("V", strlen($sheet) + 0x01);
            $sheet      = join("\0", split('', $sheet));
            $sheet     .= "\0\0\0";
        }
        else {
            $sheet_len   = '';
            $sheet       = '';
        }
    
        // Pack the link type
        $link_type   = pack("V", $link_type);
    
        // Calculate the up-level dir count e.g.. (..\..\..\ == 3)
        $up_count    = preg_match_all("/\.\.\\/", $dir_long, $useless);
        $up_count    = pack("v", $up_count);
    
        // Store the short dos dir name (null terminated)
        $dir_short   = preg_replace('/\.\.\\/', '', $dir_long) . "\0";
    
        // Store the long dir name as a wchar string (non-null terminated)
        $dir_long       = join("\0", split('', $dir_long));
        $dir_long       = $dir_long . "\0";
    
        // Pack the lengths of the dir strings
        $dir_short_len = pack("V", strlen($dir_short)      );
        $dir_long_len  = pack("V", strlen($dir_long)       );
        $stream_len    = pack("V", strlen($dir_long) + 0x06);
    
        // Pack the undocumented parts of the hyperlink stream
        $unknown1 = pack("H*",'D0C9EA79F9BACE118C8200AA004BA90B02000000'       );
        $unknown2 = pack("H*",'0303000000000000C000000000000046'               );
        $unknown3 = pack("H*",'FFFFADDE000000000000000000000000000000000000000');
        $unknown4 = pack("v",  0x03                                            );
    
        // Pack the main data stream
        $data        = pack("vvvv", $row1, $row2, $col1, $col2) .
                          $unknown1     .
                          $link_type    .
                          $unknown2     .
                          $up_count     .
                          $dir_short_len.
                          $dir_short    .
                          $unknown3     .
                          $stream_len   .
                          $dir_long_len .
                          $unknown4     .
                          $dir_long     .
                          $sheet_len    .
                          $sheet        ;
    
        // Pack the header data
        $length   = strlen($data);
        $header   = pack("vv", $record, $length);
    
        // Write the packed data
        $this->_append($header. $data);
        return($str_error);
    }
    
    
    /*
    ###############################################################################
    #
    # write_url_xxx($row1, $col1, $row2, $col2, $url, $string, $format)
    #
    # Write links to external MS/Novell network drives and shares such as
    # '//NETWORK/share/foo.xls' and '//NETWORK/share/foo.xls#Sheet1!A1'.
    #
    # See also write_url() above for a general description and return values.
    #
    sub _write_url_external_net {
    
        my $this    = shift;
    
        my $record      = 0x01B8;                       # Record identifier
        my $length      = 0x00000;                      # Bytes to follow
    
        my $row1        = $_[0];                        # Start row
        my $col1        = $_[1];                        # Start column
        my $row2        = $_[2];                        # End row
        my $col2        = $_[3];                        # End column
        my $url         = $_[4];                        # URL string
        my $str         = $_[5];                        # Alternative label
        my $xf          = $_[6] || $this->{_url_format};# The cell format
    
    
        # Strip URL type and change Unix dir separator to Dos style (if needed)
        #
        $url            =~ s[^external:][];
        $url            =~ s[/][\\]g;
    
    
        # Write the visible label
        ($str = $url)   =~ s[\#][ - ] unless defined $str;
        my $str_error   = $this->write_string($row1, $col1, $str, $xf);
        return $str_error if $str_error == -2;
    
    
        # Determine if the link contains a sheet reference and change some of the
        # parameters accordingly.
        # Split the dir name and sheet name (if it exists)
        #
        my ($dir_long , $sheet) = split /\#/, $url;
        my $link_type           = 0x0103; # Always absolute
        my $sheet_len;
    
        if (defined $sheet) {
            $link_type |= 0x08;
            $sheet_len  = pack("V", length($sheet) + 0x01);
            $sheet      = join("\0", split('', $sheet));
            $sheet     .= "\0\0\0";
    }
        else {
            $sheet_len   = '';
            $sheet       = '';
    }
    
        # Pack the link type
        $link_type      = pack("V", $link_type);
    
    
        # Make the string null terminated
        $dir_long       = $dir_long . "\0";
    
    
        # Pack the lengths of the dir string
        my $dir_long_len  = pack("V", length $dir_long);
    
    
        # Store the long dir name as a wchar string (non-null terminated)
        $dir_long       = join("\0", split('', $dir_long));
        $dir_long       = $dir_long . "\0";
    
    
        # Pack the undocumented part of the hyperlink stream
        my $unknown1    = pack("H*",'D0C9EA79F9BACE118C8200AA004BA90B02000000');
    
    
        # Pack the main data stream
        my $data        = pack("vvvv", $row1, $row2, $col1, $col2) .
                          $unknown1     .
                          $link_type    .
                          $dir_long_len .
                          $dir_long     .
                          $sheet_len    .
                          $sheet        ;
    
    
        # Pack the header data
        $length         = length $data;
        my $header      = pack("vv",   $record, $length);
    
    
        # Write the packed data
        $this->_append( $header, $data);
    
        return $str_error;
}*/
    
    /**
    * This method is used to set the height and XF format for a row.
    * Writes the  BIFF record ROW.
    *
    * @access public
    * @param integer $row    The row to set
    * @param integer $height Height we are giving to the row. 
    *                        Use NULL to set XF without setting height
    * @param mixed   $format XF format we are giving to the row
    */
    function set_row($row, $height, $format = null)
    {
        $record      = 0x0208;               // Record identifier
        $length      = 0x0010;               // Number of bytes to follow
    
        $colMic      = 0x0000;               // First defined column
        $colMac      = 0x0000;               // Last defined column
        $irwMac      = 0x0000;               // Used by Excel to optimise loading
        $reserved    = 0x0000;               // Reserved
        $grbit       = 0x01C0;               // Option flags. (monkey) see $1 do
        $ixfe        = $this->_XF($format); // XF index
    
        // Use set_row($row, NULL, $XF) to set XF without setting height
        if ($height != NULL) {
            $miyRw = $height * 20;  // row height
        }
        else {
            $miyRw = 0xff;          // default row height is 256
        }
    
        $header   = pack("vv",       $record, $length);
        $data     = pack("vvvvvvvv", $row, $colMic, $colMac, $miyRw,
                                     $irwMac,$reserved, $grbit, $ixfe);
        $this->_append($header.$data);
    }
    
    /**
    * Writes Excel DIMENSIONS to define the area in which there is data.
    */
    function _store_dimensions()
    {
        $record    = 0x0000;               // Record identifier
        $length    = 0x000A;               // Number of bytes to follow
        $row_min   = $this->dim_rowmin;    // First row
        $row_max   = $this->dim_rowmax;    // Last row plus 1
        $col_min   = $this->dim_colmin;    // First column
        $col_max   = $this->dim_colmax;    // Last column plus 1
        $reserved  = 0x0000;               // Reserved by Excel
    
        $header    = pack("vv",    $record, $length);
        $data      = pack("vvvvv", $row_min, $row_max,
                                   $col_min, $col_max, $reserved);
        $this->_prepend($header.$data);
    }
    
    /**
    * Write BIFF record Window2.
    */
    function _store_window2()
    {
        $record         = 0x023E;     // Record identifier
        $length         = 0x000A;     // Number of bytes to follow
    
        $grbit          = 0x00B6;     // Option flags
        $rwTop          = 0x0000;     // Top row visible in window
        $colLeft        = 0x0000;     // Leftmost column visible in window
        $rgbHdr         = 0x00000000; // Row/column heading and gridline color
    
        // The options flags that comprise $grbit
        $fDspFmla       = 0;                     // 0 - bit
        $fDspGrid       = 1;                     // 1
        $fDspRwCol      = 1;                     // 2
        $fFrozen        = $this->_frozen;        // 3
        $fDspZeros      = 1;                     // 4
        $fDefaultHdr    = 1;                     // 5
        $fArabic        = $this->_rtl;           // 6
        $fDspGuts       = 1;                     // 7
        $fFrozenNoSplit = 0;                     // 0 - bit
        $fSelected      = $this->selected;       // 1
        $fPaged         = 1;                     // 2
    
        $grbit             = $fDspFmla;
        $grbit            |= $fDspGrid       << 1;
        $grbit            |= $fDspRwCol      << 2;
        $grbit            |= $fFrozen        << 3;
        $grbit            |= $fDspZeros      << 4;
        $grbit            |= $fDefaultHdr    << 5;
        $grbit            |= $fArabic        << 6;
        $grbit            |= $fDspGuts       << 7;
        $grbit            |= $fFrozenNoSplit << 8;
        $grbit            |= $fSelected      << 9;
        $grbit            |= $fPaged         << 10;
    
        $header  = pack("vv",   $record, $length);
        $data    = pack("vvvV", $grbit, $rwTop, $colLeft, $rgbHdr);
        $this->_append($header.$data);
    }
    
    /**
    * Write BIFF record DEFCOLWIDTH if COLINFO records are in use.
    */
    function _store_defcol()
    {
        $record   = 0x0055;      // Record identifier
        $length   = 0x0002;      // Number of bytes to follow
        $colwidth = 0x0008;      // Default column width
    
        $header   = pack("vv", $record, $length);
        $data     = pack("v",  $colwidth);
        $this->_prepend($header.$data);
    }
    
    /**
    * Write BIFF record COLINFO to define column widths
    *
    * Note: The SDK says the record length is 0x0B but Excel writes a 0x0C
    * length record.
    *
    * @param array $col_array This is the only parameter received and is composed of the following:
    *                0 => First formatted column,
    *                1 => Last formatted column,
    *                2 => Col width (8.43 is Excel default),
    *                3 => The optional XF format of the column,
    *                4 => Option flags.
    */
    function _store_colinfo($col_array)
    {
        if(isset($col_array[0])) {
            $colFirst = $col_array[0];
        }
        if(isset($col_array[1])) {
            $colLast = $col_array[1];
        }
        if(isset($col_array[2])) {
            $coldx = $col_array[2];
        }
        else {
            $coldx = 8.43;
        }
        if(isset($col_array[3])) {
            $format = $col_array[3];
        }
        else {
            $format = null;
        }
        if(isset($col_array[4])) {
            $grbit = $col_array[4];
        }
        else {
            $grbit = 0;
        }
        $record   = 0x007D;          // Record identifier
        $length   = 0x000B;          // Number of bytes to follow
    
        $coldx   += 0.72;            // Fudge. Excel subtracts 0.72 !?
        $coldx   *= 256;             // Convert to units of 1/256 of a char
    
        $ixfe     = $this->_XF($format);
        $reserved = 0x00;            // Reserved
    
        $header   = pack("vv",     $record, $length);
        $data     = pack("vvvvvC", $colFirst, $colLast, $coldx,
                                   $ixfe, $grbit, $reserved);
        $this->_prepend($header.$data);
    }
    
    /**
    * Write BIFF record SELECTION.
    *
    * @param array $array array containing ($rwFirst,$colFirst,$rwLast,$colLast)
    * @see set_selection()
    */
    function _store_selection($array)
    {
        list($rwFirst,$colFirst,$rwLast,$colLast) = $array;
        $record   = 0x001D;                  // Record identifier
        $length   = 0x000F;                  // Number of bytes to follow
    
        $pnn      = $this->_active_pane;     // Pane position
        $rwAct    = $rwFirst;                // Active row
        $colAct   = $colFirst;               // Active column
        $irefAct  = 0;                       // Active cell ref
        $cref     = 1;                       // Number of refs
    
        if (!isset($rwLast)) {
            $rwLast   = $rwFirst;       // Last  row in reference
        }
        if (!isset($colLast)) {
            $colLast  = $colFirst;      // Last  col in reference
        }
    
        // Swap last row/col for first row/col as necessary
        if ($rwFirst > $rwLast)
        {
            list($rwFirst, $rwLast) = array($rwLast, $rwFirst);
        }
    
        if ($colFirst > $colLast)
        {
            list($colFirst, $colLast) = array($colLast, $colFirst);
        }
    
        $header   = pack("vv",         $record, $length);
        $data     = pack("CvvvvvvCC",  $pnn, $rwAct, $colAct,
                                       $irefAct, $cref,
                                       $rwFirst, $rwLast,
                                       $colFirst, $colLast);
        $this->_append($header.$data);
    }
    
    
    /**
    * Write BIFF record EXTERNCOUNT to indicate the number of external sheet
    * references in a worksheet.
    *
    * Excel only stores references to external sheets that are used in formulas.
    * For simplicity we store references to all the sheets in the workbook
    * regardless of whether they are used or not. This reduces the overall
    * complexity and eliminates the need for a two way dialogue between the formula
    * parser the worksheet objects.
    *
    * @param integer $count The number of external sheet references in this worksheet
    */
    function _store_externcount($count)
    {
        $record   = 0x0016;          // Record identifier
        $length   = 0x0002;          // Number of bytes to follow
    
        $header   = pack("vv", $record, $length);
        $data     = pack("v",  $count);
        $this->_prepend($header.$data);
    }
    
    /**
    * Writes the Excel BIFF EXTERNSHEET record. These references are used by
    * formulas. A formula references a sheet name via an index. Since we store a
    * reference to all of the external worksheets the EXTERNSHEET index is the same
    * as the worksheet index.
    *
    * @param string $sheetname The name of a external worksheet
    */
    function _store_externsheet($sheetname)
    {
        $record    = 0x0017;         // Record identifier
    
        // References to the current sheet are encoded differently to references to
        // external sheets.
        //
        if ($this->name == $sheetname) {
            $sheetname = '';
            $length    = 0x02;  // The following 2 bytes
            $cch       = 1;     // The following byte
            $rgch      = 0x02;  // Self reference
        }
        else {
            $length    = 0x02 + strlen($sheetname);
            $cch       = strlen($sheetname);
            $rgch      = 0x03;  // Reference to a sheet in the current workbook
        }
    
        $header     = pack("vv",  $record, $length);
        $data       = pack("CC", $cch, $rgch);
        $this->_prepend($header.$data.$sheetname);
    }
    
    /**
    * Writes the Excel BIFF PANE record.
    * The panes can either be frozen or thawed (unfrozen).
    * Frozen panes are specified in terms of an integer number of rows and columns.
    * Thawed panes are specified in terms of Excel's units for rows and columns.
    *
    * @param array $panes This is the only parameter received and is composed of the following:
    *                     0 => Vertical split position,
    *                     1 => Horizontal split position
    *                     2 => Top row visible
    *                     3 => Leftmost column visible
    *                     4 => Active pane
    */
    function _store_panes($panes)
    {
        $y       = $panes[0];
        $x       = $panes[1];
        $rwTop   = $panes[2];
        $colLeft = $panes[3];
        if(count($panes) > 4) { // if Active pane was received
            $pnnAct = $panes[4];
        }
        else {
            $pnnAct = NULL;
        }
        $record  = 0x0041;       // Record identifier
        $length  = 0x000A;       // Number of bytes to follow
    
        // Code specific to frozen or thawed panes.
        if ($this->_frozen) {
            // Set default values for $rwTop and $colLeft
            if(!isset($rwTop)) {
                $rwTop   = $y;
            }
            if(!isset($colLeft)) {
                $colLeft = $x;
            }
        }
        else {
            // Set default values for $rwTop and $colLeft
            if(!isset($rwTop)) {
                $rwTop   = 0;
            }
            if(!isset($colLeft)) {
                $colLeft = 0;
            }
    
            // Convert Excel's row and column units to the internal units.
            // The default row height is 12.75
            // The default column width is 8.43
            // The following slope and intersection values were interpolated.
            //
            $y = 20*$y      + 255;
            $x = 113.879*$x + 390;
        }
    
    
        // Determine which pane should be active. There is also the undocumented
        // option to override this should it be necessary: may be removed later.
        //
        if (!isset($pnnAct))
        {
            if ($x != 0 and $y != 0)
                $pnnAct = 0; // Bottom right
            if ($x != 0 and $y == 0)
                $pnnAct = 1; // Top right
            if ($x == 0 and $y != 0)
                $pnnAct = 2; // Bottom left
            if ($x == 0 and $y == 0)
                $pnnAct = 3; // Top left
        }
    
        $this->_active_pane = $pnnAct; // Used in _store_selection
    
        $header     = pack("vv",    $record, $length);
        $data       = pack("vvvvv", $x, $y, $rwTop, $colLeft, $pnnAct);
        $this->_append($header.$data);
    }
    
    /**
    * Store the page setup SETUP BIFF record.
    */
    function _store_setup()
    {
        $record       = 0x00A1;                  // Record identifier
        $length       = 0x0022;                  // Number of bytes to follow
    
        $iPaperSize   = $this->_paper_size;    // Paper size
        $iScale       = $this->_print_scale;   // Print scaling factor
        $iPageStart   = 0x01;                 // Starting page number
        $iFitWidth    = $this->_fit_width;    // Fit to number of pages wide
        $iFitHeight   = $this->_fit_height;   // Fit to number of pages high
        $grbit        = 0x00;                 // Option flags
        $iRes         = 0x0258;               // Print resolution
        $iVRes        = 0x0258;               // Vertical print resolution
        $numHdr       = $this->_margin_head;  // Header Margin
        $numFtr       = $this->_margin_foot;   // Footer Margin
        $iCopies      = 0x01;                 // Number of copies
    
        $fLeftToRight = 0x0;                     // Print over then down
        $fLandscape   = $this->_orientation;     // Page orientation
        $fNoPls       = 0x0;                     // Setup not read from printer
        $fNoColor     = 0x0;                     // Print black and white
        $fDraft       = 0x0;                     // Print draft quality
        $fNotes       = 0x0;                     // Print notes
        $fNoOrient    = 0x0;                     // Orientation not set
        $fUsePage     = 0x0;                     // Use custom starting page
    
        $grbit           = $fLeftToRight;
        $grbit          |= $fLandscape    << 1;
        $grbit          |= $fNoPls        << 2;
        $grbit          |= $fNoColor      << 3;
        $grbit          |= $fDraft        << 4;
        $grbit          |= $fNotes        << 5;
        $grbit          |= $fNoOrient     << 6;
        $grbit          |= $fUsePage      << 7;
    
        $numHdr = pack("d", $numHdr);
        $numFtr = pack("d", $numFtr);
        if ($this->_byte_order) // if it's Big Endian
        {
            $numHdr = strrev($numHdr);
            $numFtr = strrev($numFtr);
        }
    
        $header = pack("vv", $record, $length);
        $data1  = pack("vvvvvvvv", $iPaperSize,
                                   $iScale,
                                   $iPageStart,
                                   $iFitWidth,
                                   $iFitHeight,
                                   $grbit,
                                   $iRes,
                                   $iVRes);
        $data2  = $numHdr .$numFtr;
        $data3  = pack("v", $iCopies);
        $this->_prepend($header.$data1.$data2.$data3);
    }
    
    /**
    * Store the header caption BIFF record.
    */
    function store_header()
    {
        $record  = 0x0014;               // Record identifier
    
        $str     = $this->_header;        // header string
        $cch     = strlen($str);         // Length of header string
        $length  = 1 + $cch;             // Bytes to follow
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("C",   $cch);
    
        $this->_append($header.$data.$str);
    }
    
    /**
    * Store the footer caption BIFF record.
    */
    function store_footer()
    {
        $record  = 0x0015;               // Record identifier
    
        $str     = $this->_footer;       // Footer string
        $cch     = strlen($str);         // Length of footer string
        $length  = 1 + $cch;             // Bytes to follow
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("C",   $cch);
    
        $this->_append($header.$data.$str);
    }
    
    /**
    * Store the horizontal centering HCENTER BIFF record.
    */
    function store_hcenter()
    {
        $record   = 0x0083;              // Record identifier
        $length   = 0x0002;              // Bytes to follow
    
        $fHCenter = $this->_hcenter;      // Horizontal centering
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("v",   $fHCenter);
    
        $this->_append($header.$data);
    }
    
    /**
    * Store the vertical centering VCENTER BIFF record.
    */
    function store_vcenter()
    {
        $record   = 0x0084;              // Record identifier
        $length   = 0x0002;              // Bytes to follow
    
        $fVCenter = $this->_vcenter;      // Horizontal centering
    
        $header    = pack("vv", $record, $length);
        $data      = pack("v", $fVCenter);
        $this->_append($header.$data);
    }
    
    /**
    * Store the LEFTMARGIN BIFF record.
    */
    function _store_margin_left()
    {
        $record  = 0x0026;                   // Record identifier
        $length  = 0x0008;                   // Bytes to follow
    
        $margin  = $this->_margin_left;       // Margin in inches
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("d",   $margin);
        if ($this->_byte_order) // if it's Big Endian
        { 
            $data = strrev($data);
        }
    
        $this->_append($header.$data);
    }
    
    /**
    * Store the RIGHTMARGIN BIFF record.
    */
    function _store_margin_right()
    {
        $record  = 0x0027;                   // Record identifier
        $length  = 0x0008;                   // Bytes to follow
    
        $margin  = $this->_margin_right;      // Margin in inches
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("d",   $margin);
        if ($this->_byte_order) // if it's Big Endian
        { 
            $data = strrev($data);
        }
    
        $this->_append($header.$data);
    }
    
    /**
    * Store the TOPMARGIN BIFF record.
    */
    function _store_margin_top()
    {
        $record  = 0x0028;                   // Record identifier
        $length  = 0x0008;                   // Bytes to follow
    
        $margin  = $this->_margin_top;        // Margin in inches
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("d",   $margin);
        if ($this->_byte_order) // if it's Big Endian
        { 
            $data = strrev($data);
        }
    
        $this->_append($header.$data);
    }
    
    /**
    * Store the BOTTOMMARGIN BIFF record.
    */
    function _store_margin_bottom()
    {
        $record  = 0x0029;                   // Record identifier
        $length  = 0x0008;                   // Bytes to follow
    
        $margin  = $this->_margin_bottom;     // Margin in inches
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("d",   $margin);
        if ($this->_byte_order) // if it's Big Endian
        { 
            $data = strrev($data);
        }
    
        $this->_append($header.$data);
    }

    /**
    * This is an Excel97/2000 method. It is required to perform more complicated
    * merging than the normal set_align('merge'). It merges the area given by 
    * its arguments.
    *
    * @access public
    * @param integer $first_row First row of the area to merge
    * @param integer $first_col First column of the area to merge
    * @param integer $last_row  Last row of the area to merge
    * @param integer $last_col  Last column of the area to merge
    */
    function merge_cells($first_row, $first_col, $last_row, $last_col)
    {
        $record  = 0x00E5;                   // Record identifier
        $length  = 0x000A;                   // Bytes to follow
        $cref     = 1;                       // Number of refs

        // Swap last row/col for first row/col as necessary
        if ($first_row > $last_row) {
            list($first_row, $last_row) = array($last_row, $first_row);
        }
    
        if ($first_col > $last_col) {
            list($first_col, $last_col) = array($last_col, $first_col);
        }
    
        $header   = pack("vv",    $record, $length);
        $data     = pack("vvvvv", $cref, $first_row, $last_row,
                                  $first_col, $last_col);
    
        $this->_append($header.$data);
    }
    
    /**
    * Write the PRINTHEADERS BIFF record.
    */
    function _store_print_headers()
    {
        $record      = 0x002a;                   // Record identifier
        $length      = 0x0002;                   // Bytes to follow
    
        $fPrintRwCol = $this->_print_headers;     // Boolean flag
    
        $header      = pack("vv", $record, $length);
        $data        = pack("v", $fPrintRwCol);
        $this->_prepend($header.$data);
    }
    
    /**
    * Write the PRINTGRIDLINES BIFF record. Must be used in conjunction with the
    * GRIDSET record.
    */
    function _store_print_gridlines()
    {
        $record      = 0x002b;                    // Record identifier
        $length      = 0x0002;                    // Bytes to follow
    
        $fPrintGrid  = $this->_print_gridlines;    // Boolean flag
    
        $header      = pack("vv", $record, $length);
        $data        = pack("v", $fPrintGrid);
        $this->_prepend($header.$data);
    }
    
    /**
    * Write the GRIDSET BIFF record. Must be used in conjunction with the
    * PRINTGRIDLINES record.
    */
    function _store_gridset()
    {
        $record      = 0x0082;                        // Record identifier
        $length      = 0x0002;                        // Bytes to follow
    
        $fGridSet    = !($this->_print_gridlines);     // Boolean flag
    
        $header      = pack("vv",  $record, $length);
        $data        = pack("v",   $fGridSet);
        $this->_prepend($header.$data);
    }
    
    /**
    * Write the WSBOOL BIFF record, mainly for fit-to-page. Used in conjunction
    * with the SETUP record.
    */
    function _store_wsbool()
    {
        $record      = 0x0081;   // Record identifier
        $length      = 0x0002;   // Bytes to follow
    
        // The only option that is of interest is the flag for fit to page. So we
        // set all the options in one go.
        //
        if ($this->_fit_page) {
            $grbit = 0x05c1;
        }
        else {
            $grbit = 0x04c1;
        }
    
        $header      = pack("vv", $record, $length);
        $data        = pack("v",  $grbit);
        $this->_prepend($header.$data);
    }
    
    
    /**
    * Write the HORIZONTALPAGEBREAKS BIFF record.
    */
    function _store_hbreak()
    {
        // Return if the user hasn't specified pagebreaks
        if(empty($this->_hbreaks)) {
            return;
        }
    
        // Sort and filter array of page breaks
        $breaks = $this->_hbreaks;
        sort($breaks,SORT_NUMERIC);
        if($breaks[0] == 0) { // don't use first break if it's 0
            array_shift($breaks);
        }
    
        $record  = 0x001b;               // Record identifier
        $cbrk    = count($breaks);       // Number of page breaks
        $length  = ($cbrk + 1) * 2;      // Bytes to follow
    
        $header  = pack("vv", $record, $length);
        $data    = pack("v",  $cbrk);
    
        // Append each page break
        foreach($breaks as $break) {
            $data .= pack("v", $break);
        }
    
        $this->_prepend($header.$data);
    }
    
    
    /**
    * Write the VERTICALPAGEBREAKS BIFF record.
    */
    function _store_vbreak()
    {
        // Return if the user hasn't specified pagebreaks
        if(empty($this->_vbreaks)) {
            return;
        }
    
        // 1000 vertical pagebreaks appears to be an internal Excel 5 limit.
        // It is slightly higher in Excel 97/200, approx. 1026
        $breaks = array_slice($this->_vbreaks,0,1000);
    
        // Sort and filter array of page breaks
        sort($breaks,SORT_NUMERIC);
        if($breaks[0] == 0) { // don't use first break if it's 0
            array_shift($breaks);
        }
    
        $record  = 0x001a;               // Record identifier
        $cbrk    = count($breaks);       // Number of page breaks
        $length  = ($cbrk + 1) * 2;      // Bytes to follow
    
        $header  = pack("vv",  $record, $length);
        $data    = pack("v",   $cbrk);
    
        // Append each page break
        foreach ($breaks as $break) {
            $data .= pack("v", $break);
        }
    
        $this->_prepend($header.$data);
    }
    
    /**
    * Set the Biff PROTECT record to indicate that the worksheet is protected.
    */
    function _store_protect()
    {
        // Exit unless sheet protection has been specified
        if($this->_protect == 0) {
            return;
        }
    
        $record      = 0x0012;             // Record identifier
        $length      = 0x0002;             // Bytes to follow
    
        $fLock       = $this->_protect;    // Worksheet is protected
    
        $header      = pack("vv", $record, $length);
        $data        = pack("v",  $fLock);
    
        $this->_prepend($header.$data);
    }
    
    /**
    * Write the worksheet PASSWORD record.
    */
    function _store_password()
    {
        // Exit unless sheet protection and password have been specified
        if(($this->_protect == 0) or (!isset($this->_password))) {
            return;
        }
    
        $record      = 0x0013;               // Record identifier
        $length      = 0x0002;               // Bytes to follow
    
        $wPassword   = $this->_password;     // Encoded password
    
        $header      = pack("vv", $record, $length);
        $data        = pack("v",  $wPassword);
    
        $this->_prepend($header.$data);
    }
    
    /**
    * Insert a 24bit bitmap image in a worksheet. The main record required is
    * IMDATA but it must be proceeded by a OBJ record to define its position.
    *
    * @access public
    * @param integer $row     The row we are going to insert the bitmap into
    * @param integer $col     The column we are going to insert the bitmap into
    * @param string  $bitmap  The bitmap filename
    * @param integer $x       The horizontal position (offset) of the image inside the cell.
    * @param integer $y       The vertical position (offset) of the image inside the cell.
    * @param integer $scale_x The horizontal scale
    * @param integer $scale_y The vertical scale
    */
    function insert_bitmap($row, $col, $bitmap, $x = 0, $y = 0, $scale_x = 1, $scale_y = 1)
    {
        list($width, $height, $size, $data) = $this->_process_bitmap($bitmap);
    
        // Scale the frame of the image.
        $width  *= $scale_x;
        $height *= $scale_y;
    
        // Calculate the vertices of the image and write the OBJ record
        $this->_position_image($col, $row, $x, $y, $width, $height);
    
        // Write the IMDATA record to store the bitmap data
        $record      = 0x007f;
        $length      = 8 + $size;
        $cf          = 0x09;
        $env         = 0x01;
        $lcb         = $size;
    
        $header      = pack("vvvvV", $record, $length, $cf, $env, $lcb);
        $this->_append($header.$data);
    }
    
    /**
    * Calculate the vertices that define the position of the image as required by
    * the OBJ record.
    *
    *         +------------+------------+
    *         |     A      |      B     |
    *   +-----+------------+------------+
    *   |     |(x1,y1)     |            |
    *   |  1  |(A1)._______|______      |
    *   |     |    |              |     |
    *   |     |    |              |     |
    *   +-----+----|    BITMAP    |-----+
    *   |     |    |              |     |
    *   |  2  |    |______________.     |
    *   |     |            |        (B2)|
    *   |     |            |     (x2,y2)|
    *   +---- +------------+------------+
    *
    * Example of a bitmap that covers some of the area from cell A1 to cell B2.
    *
    * Based on the width and height of the bitmap we need to calculate 8 vars:
    *     $col_start, $row_start, $col_end, $row_end, $x1, $y1, $x2, $y2.
    * The width and height of the cells are also variable and have to be taken into
    * account.
    * The values of $col_start and $row_start are passed in from the calling
    * function. The values of $col_end and $row_end are calculated by subtracting
    * the width and height of the bitmap from the width and height of the
    * underlying cells.
    * The vertices are expressed as a percentage of the underlying cell width as
    * follows (rhs values are in pixels):
    *
    *       x1 = X / W *1024
    *       y1 = Y / H *256
    *       x2 = (X-1) / W *1024
    *       y2 = (Y-1) / H *256
    *
    *       Where:  X is distance from the left side of the underlying cell
    *               Y is distance from the top of the underlying cell
    *               W is the width of the cell
    *               H is the height of the cell
    *
    * @note  the SDK incorrectly states that the height should be expressed as a
    *        percentage of 1024.
    * @param integer $col_start Col containing upper left corner of object
    * @param integer $row_start Row containing top left corner of object
    * @param integer $x1        Distance to left side of object
    * @param integer $y1        Distance to top of object
    * @param integer $width     Width of image frame
    * @param integer $height    Height of image frame
    */
    function _position_image($col_start, $row_start, $x1, $y1, $width, $height)
    {
        // Initialise end cell to the same as the start cell
        $col_end    = $col_start;  // Col containing lower right corner of object
        $row_end    = $row_start;  // Row containing bottom right corner of object
    
        // Zero the specified offset if greater than the cell dimensions
        if ($x1 >= $this->size_col($col_start))
        {
            $x1 = 0;
        }
        if ($y1 >= $this->size_row($row_start))
        {
            $y1 = 0;
        }
    
        $width      = $width  + $x1 -1;
        $height     = $height + $y1 -1;
    
        // Subtract the underlying cell widths to find the end cell of the image
        while ($width >= $this->size_col($col_end)) {
            $width -= $this->size_col($col_end);
            $col_end++;
        }
    
        // Subtract the underlying cell heights to find the end cell of the image
        while ($height >= $this->size_row($row_end)) {
            $height -= $this->size_row($row_end);
            $row_end++;
        }
    
        // Bitmap isn't allowed to start or finish in a hidden cell, i.e. a cell
        // with zero eight or width.
        //
        if ($this->size_col($col_start) == 0)
            return;
        if ($this->size_col($col_end)   == 0)
            return;
        if ($this->size_row($row_start) == 0)
            return;
        if ($this->size_row($row_end)   == 0)
            return;
    
        // Convert the pixel values to the percentage value expected by Excel
        $x1 = $x1     / $this->size_col($col_start)   * 1024;
        $y1 = $y1     / $this->size_row($row_start)   *  256;
        $x2 = $width  / $this->size_col($col_end)     * 1024; // Distance to right side of object
        $y2 = $height / $this->size_row($row_end)     *  256; // Distance to bottom of object
    
        $this->_store_obj_picture( $col_start, $x1,
                                  $row_start, $y1,
                                  $col_end, $x2,
                                  $row_end, $y2
                                );
    }
    
    /**
    * Convert the width of a cell from user's units to pixels. By interpolation
    * the relationship is: y = 7x +5. If the width hasn't been set by the user we
    * use the default value. If the col is hidden we use a value of zero.
    *
    * @param integer  $col The column 
    * @return integer The width in pixels
    */
    function size_col($col)
    {
        // Look up the cell value to see if it has been changed
        if (isset($this->col_sizes[$col])) {
            if ($this->col_sizes[$col] == 0) {
                return(0);
            }
            else {
                return(floor(7 * $this->col_sizes[$col] + 5));
            }
        }
        else {
            return(64);
        }
    }
    
    /**
    * Convert the height of a cell from user's units to pixels. By interpolation
    * the relationship is: y = 4/3x. If the height hasn't been set by the user we
    * use the default value. If the row is hidden we use a value of zero. (Not
    * possible to hide row yet).
    *
    * @param integer $row The row
    * @return integer The width in pixels
    */
    function size_row($row)
    {
        // Look up the cell value to see if it has been changed
        if (isset($this->row_sizes[$row])) {
            if ($this->row_sizes[$row] == 0) {
                return(0);
            }
            else {
                return(floor(4/3 * $this->row_sizes[$row]));
            }
        }
        else {
            return(17);
        }
    }
    
    /**
    * Store the OBJ record that precedes an IMDATA record. This could be generalise
    * to support other Excel objects.
    *
    * @param integer $colL Column containing upper left corner of object
    * @param integer $dxL  Distance from left side of cell
    * @param integer $rwT  Row containing top left corner of object
    * @param integer $dyT  Distance from top of cell
    * @param integer $colR Column containing lower right corner of object
    * @param integer $dxR  Distance from right of cell
    * @param integer $rwB  Row containing bottom right corner of object
    * @param integer $dyB  Distance from bottom of cell
    */
    function _store_obj_picture($colL,$dxL,$rwT,$dyT,$colR,$dxR,$rwB,$dyB)
    {
        $record      = 0x005d;   // Record identifier
        $length      = 0x003c;   // Bytes to follow
    
        $cObj        = 0x0001;   // Count of objects in file (set to 1)
        $OT          = 0x0008;   // Object type. 8 = Picture
        $id          = 0x0001;   // Object ID
        $grbit       = 0x0614;   // Option flags
    
        $cbMacro     = 0x0000;   // Length of FMLA structure
        $Reserved1   = 0x0000;   // Reserved
        $Reserved2   = 0x0000;   // Reserved
    
        $icvBack     = 0x09;     // Background colour
        $icvFore     = 0x09;     // Foreground colour
        $fls         = 0x00;     // Fill pattern
        $fAuto       = 0x00;     // Automatic fill
        $icv         = 0x08;     // Line colour
        $lns         = 0xff;     // Line style
        $lnw         = 0x01;     // Line weight
        $fAutoB      = 0x00;     // Automatic border
        $frs         = 0x0000;   // Frame style
        $cf          = 0x0009;   // Image format, 9 = bitmap
        $Reserved3   = 0x0000;   // Reserved
        $cbPictFmla  = 0x0000;   // Length of FMLA structure
        $Reserved4   = 0x0000;   // Reserved
        $grbit2      = 0x0001;   // Option flags
        $Reserved5   = 0x0000;   // Reserved
    
    
        $header      = pack("vv", $record, $length);
        $data        = pack("V", $cObj);
        $data       .= pack("v", $OT);
        $data       .= pack("v", $id);
        $data       .= pack("v", $grbit);
        $data       .= pack("v", $colL);
        $data       .= pack("v", $dxL);
        $data       .= pack("v", $rwT);
        $data       .= pack("v", $dyT);
        $data       .= pack("v", $colR);
        $data       .= pack("v", $dxR);
        $data       .= pack("v", $rwB);
        $data       .= pack("v", $dyB);
        $data       .= pack("v", $cbMacro);
        $data       .= pack("V", $Reserved1);
        $data       .= pack("v", $Reserved2);
        $data       .= pack("C", $icvBack);
        $data       .= pack("C", $icvFore);
        $data       .= pack("C", $fls);
        $data       .= pack("C", $fAuto);
        $data       .= pack("C", $icv);
        $data       .= pack("C", $lns);
        $data       .= pack("C", $lnw);
        $data       .= pack("C", $fAutoB);
        $data       .= pack("v", $frs);
        $data       .= pack("V", $cf);
        $data       .= pack("v", $Reserved3);
        $data       .= pack("v", $cbPictFmla);
        $data       .= pack("v", $Reserved4);
        $data       .= pack("v", $grbit2);
        $data       .= pack("V", $Reserved5);
    
        $this->_append($header.$data);
    }
    
    /**
    * Convert a 24 bit bitmap into the modified internal format used by Windows.
    * This is described in BITMAPCOREHEADER and BITMAPCOREINFO structures in the
    * MSDN library.
    *
    * @param string $bitmap The bitmap to process
    * @return array Array with data and properties of the bitmap
    */
    function _process_bitmap($bitmap)
    {
        // Open file.
        $bmp_fd = fopen($bitmap,"rb");
        if (!$bmp_fd) {
            die("Couldn't import $bitmap");
        }
            
        // Slurp the file into a string.
        $data = fread($bmp_fd, filesize($bitmap));
    
        // Check that the file is big enough to be a bitmap.
        if (strlen($data) <= 0x36) {
            die("$bitmap doesn't contain enough data.\n");
        }
    
        // The first 2 bytes are used to identify the bitmap.
        $identity = unpack("A2", $data);
        if ($identity[''] != "BM") {
            die("$bitmap doesn't appear to be a valid bitmap image.\n");
        }
    
        // Remove bitmap data: ID.
        $data = substr($data, 2);
    
        // Read and remove the bitmap size. This is more reliable than reading
        // the data size at offset 0x22.
        //
        $size_array   = unpack("V", substr($data, 0, 4));
        $size   = $size_array[''];
        $data   = substr($data, 4);
        $size  -= 0x36; // Subtract size of bitmap header.
        $size  += 0x0C; // Add size of BIFF header.
    
        // Remove bitmap data: reserved, offset, header length.
        $data = substr($data, 12);
    
        // Read and remove the bitmap width and height. Verify the sizes.
        $width_and_height = unpack("V2", substr($data, 0, 8));
        $width  = $width_and_height[1];
        $height = $width_and_height[2];
        $data   = substr($data, 8);
        if ($width > 0xFFFF) { 
            die("$bitmap: largest image width supported is 65k.\n");
        }
        if ($height > 0xFFFF) { 
            die("$bitmap: largest image height supported is 65k.\n");
        }
    
        // Read and remove the bitmap planes and bpp data. Verify them.
        $planes_and_bitcount = unpack("v2", substr($data, 0, 4));
        $data = substr($data, 4);
        if ($planes_and_bitcount[2] != 24) { // Bitcount
            die("$bitmap isn't a 24bit true color bitmap.\n");
        }
        if ($planes_and_bitcount[1] != 1) {
            die("$bitmap: only 1 plane supported in bitmap image.\n");
        }
    
        // Read and remove the bitmap compression. Verify compression.
        $compression = unpack("V", substr($data, 0, 4));
        $data = substr($data, 4);
      
        //$compression = 0;
        if ($compression[""] != 0) {
            die("$bitmap: compression not supported in bitmap image.\n");
        }
    
        // Remove bitmap data: data size, hres, vres, colours, imp. colours.
        $data = substr($data, 20);
    
        // Add the BITMAPCOREHEADER data
        $header  = pack("Vvvvv", 0x000c, $width, $height, 0x01, 0x18);
        $data    = $header . $data;
    
        return (array($width, $height, $size, $data));
    }
    
    /**
    * Store the window zoom factor. This should be a reduced fraction but for
    * simplicity we will store all fractions with a numerator of 100.
    */
    function _store_zoom()
    {
        // If scale is 100 we don't need to write a record
        if ($this->_zoom == 100) {
            return;
        }
    
        $record      = 0x00A0;               // Record identifier
        $length      = 0x0004;               // Bytes to follow
    
        $header      = pack("vv", $record, $length);
        $data        = pack("vv", $this->_zoom, 100);
        $this->_append($header.$data);
    }
}

/**
* Class for generating Excel Spreadsheets
*
* @author Xavier Noguer <xnoguer@rezebra.com>
* @package Spreadsheet_WriteExcel
*/

class Workbook extends BIFFwriter
{
    /**
    * Class constructor
    *
    * @param string filename for storing the workbook. "-" for writing to stdout.
    */
    function Workbook($filename)
    {
        $this->BIFFwriter(); // It needs to call its parent's constructor explicitly
    
        $this->_filename         = $filename;
        $this->parser            = new Parser($this->_byte_order);
        $this->_1904             = 0;
        $this->activesheet       = 0;
        $this->firstsheet        = 0;
        $this->selected          = 0;
        $this->xf_index          = 16; // 15 style XF's and 1 cell XF.
        $this->_fileclosed       = 0;
        $this->_biffsize         = 0;
        $this->sheetname         = "Sheet";
        $this->tmp_format        = new Format();
        $this->worksheets        = array();
        $this->sheetnames        = array();
        $this->formats           = array();
        $this->palette           = array();
    
        // Add the default format for hyperlinks
        $this->url_format =& $this->add_format(array('color' => 'blue', 'underline' => 1));
    
        // Check for a filename
        //if ($this->_filename == '') {
        //    die('Filename required by Spreadsheet::WriteExcel->new()');
        //}
    
        # Warn if tmpfiles can't be used.
        //$this->tmpfile_warning();
        $this->_set_palette_xl97();
    }
    
    /**
    * Calls finalization methods and explicitly close the OLEwriter file
    * handle.
    */
    function close()
    {
        if ($this->_fileclosed) { // Prevent close() from being called twice.
            return;
        }
        $this->store_workbook();
        $this->_fileclosed = 1;
    }
    
    
    /**
    * An accessor for the _worksheets[] array
    * Returns an array of the worksheet objects in a workbook
    *
    * @return array
    */
    function sheets()
    {
        return($this->worksheets());
    }
    
    /**
    * An accessor for the _worksheets[] array.
    *
    * @return array
    */
    function worksheets()
    {
        return($this->worksheets);
    }
    
    /**
    * Add a new worksheet to the Excel workbook.
    * TODO: Add accessor for $this->{_sheetname} for international Excel versions.
    *
    * @access public
    * @param string $name the optional name of the worksheet
    * @return &object reference to a worksheet object
    */
    function &add_worksheet($name = '')
    {
        $index     = count($this->worksheets);
        $sheetname = $this->sheetname;

        if($name == '') {
            $name = $sheetname.($index+1); 
        }
    
        // Check that sheetname is <= 31 chars (Excel limit).
        if(strlen($name) > 31) {
            die("Sheetname $name must be <= 31 chars");
        }
    
        // Check that the worksheet name doesn't already exist: a fatal Excel error.
        for($i=0; $i < count($this->worksheets); $i++)
        {
            if($name == $this->worksheets[$i]->get_name()) {
                die("Worksheet '$name' already exists");
            }
        }
    
        $worksheet = new Worksheet($name,$index,$this->activesheet,
                                   $this->firstsheet,$this->url_format,
                                   $this->parser);
        $this->worksheets[$index] = &$worksheet;      // Store ref for iterator
        $this->sheetnames[$index] = $name;            // Store EXTERNSHEET names
        //$this->parser->set_ext_sheet($name,$index); // Store names in Formula.php
        return($worksheet);
    }
    
    /**
    * DEPRECATED!! Use add_worksheet instead
    *
    * @access public
    * @deprecated Use add_worksheet instead
    * @param string $name the optional name of the worksheet
    * @return &object reference to a worksheet object
    */
    function &addworksheet($name = '')
    {
        return($this->add_worksheet($name));
    }
    
    /**
    * Add a new format to the Excel workbook. This adds an XF record and
    * a FONT record. Also, pass any properties to the Format constructor.
    *
    * @access public
    * @param array $properties array with properties for initializing the format (see Format.php)
    * @return &object reference to an XF format
    */
    function &add_format($properties = array())
    {
        $format = new Format($this->xf_index,$properties);
        $this->xf_index += 1;
        $this->formats[] = &$format;
        return($format);
    }
    
    /**
    * DEPRECATED!! Use add_format instead
    *
    * @access public
    * @deprecated Use add_format instead
    * @param array $properties array with properties for initializing the format (see Format.php)
    * @return &object reference to an XF format
    */
    function &addformat($properties = array())
    {
         return($this->add_format($properties));
    }
    
    
    /**
    * Change the RGB components of the elements in the colour palette.
    *
    * @access public
    * @param integer $index colour index
    * @param integer $red   red RGB value [0-255]
    * @param integer $green green RGB value [0-255]
    * @param integer $blue  blue RGB value [0-255]
    * @return integer The palette index for the custom color
    */
    function set_custom_color($index,$red,$green,$blue)
    {
        // Match a HTML #xxyyzz style parameter
        /*if (defined $_[1] and $_[1] =~ /^#(\w\w)(\w\w)(\w\w)/ ) {
            @_ = ($_[0], hex $1, hex $2, hex $3);
        }*/
    
        // Check that the colour index is the right range
        if ($index < 8 or $index > 64) {
            die("Color index $index outside range: 8 <= index <= 64");
        }
    
        // Check that the colour components are in the right range
        if ( ($red   < 0 or $red   > 255) ||
             ($green < 0 or $green > 255) ||
             ($blue  < 0 or $blue  > 255) )  
        {
            die("Color component outside range: 0 <= color <= 255");
        }

        $index -= 8; // Adjust colour index (wingless dragonfly)
        
        // Set the RGB value
        $this->palette[$index] = array($red, $green, $blue, 0);
        return($index + 8);
    }
    
    /**
    * Sets the colour palette to the Excel 97+ default.
    */
    function _set_palette_xl97()
    {
        $this->palette = array(
                           array(0x00, 0x00, 0x00, 0x00),   // 8
                           array(0xff, 0xff, 0xff, 0x00),   // 9
                           array(0xff, 0x00, 0x00, 0x00),   // 10
                           array(0x00, 0xff, 0x00, 0x00),   // 11
                           array(0x00, 0x00, 0xff, 0x00),   // 12
                           array(0xff, 0xff, 0x00, 0x00),   // 13
                           array(0xff, 0x00, 0xff, 0x00),   // 14
                           array(0x00, 0xff, 0xff, 0x00),   // 15
                           array(0x80, 0x00, 0x00, 0x00),   // 16
                           array(0x00, 0x80, 0x00, 0x00),   // 17
                           array(0x00, 0x00, 0x80, 0x00),   // 18
                           array(0x80, 0x80, 0x00, 0x00),   // 19
                           array(0x80, 0x00, 0x80, 0x00),   // 20
                           array(0x00, 0x80, 0x80, 0x00),   // 21
                           array(0xc0, 0xc0, 0xc0, 0x00),   // 22
                           array(0x80, 0x80, 0x80, 0x00),   // 23
                           array(0x99, 0x99, 0xff, 0x00),   // 24
                           array(0x99, 0x33, 0x66, 0x00),   // 25
                           array(0xff, 0xff, 0xcc, 0x00),   // 26
                           array(0xcc, 0xff, 0xff, 0x00),   // 27
                           array(0x66, 0x00, 0x66, 0x00),   // 28
                           array(0xff, 0x80, 0x80, 0x00),   // 29
                           array(0x00, 0x66, 0xcc, 0x00),   // 30
                           array(0xcc, 0xcc, 0xff, 0x00),   // 31
                           array(0x00, 0x00, 0x80, 0x00),   // 32
                           array(0xff, 0x00, 0xff, 0x00),   // 33
                           array(0xff, 0xff, 0x00, 0x00),   // 34
                           array(0x00, 0xff, 0xff, 0x00),   // 35
                           array(0x80, 0x00, 0x80, 0x00),   // 36
                           array(0x80, 0x00, 0x00, 0x00),   // 37
                           array(0x00, 0x80, 0x80, 0x00),   // 38
                           array(0x00, 0x00, 0xff, 0x00),   // 39
                           array(0x00, 0xcc, 0xff, 0x00),   // 40
                           array(0xcc, 0xff, 0xff, 0x00),   // 41
                           array(0xcc, 0xff, 0xcc, 0x00),   // 42
                           array(0xff, 0xff, 0x99, 0x00),   // 43
                           array(0x99, 0xcc, 0xff, 0x00),   // 44
                           array(0xff, 0x99, 0xcc, 0x00),   // 45
                           array(0xcc, 0x99, 0xff, 0x00),   // 46
                           array(0xff, 0xcc, 0x99, 0x00),   // 47
                           array(0x33, 0x66, 0xff, 0x00),   // 48
                           array(0x33, 0xcc, 0xcc, 0x00),   // 49
                           array(0x99, 0xcc, 0x00, 0x00),   // 50
                           array(0xff, 0xcc, 0x00, 0x00),   // 51
                           array(0xff, 0x99, 0x00, 0x00),   // 52
                           array(0xff, 0x66, 0x00, 0x00),   // 53
                           array(0x66, 0x66, 0x99, 0x00),   // 54
                           array(0x96, 0x96, 0x96, 0x00),   // 55
                           array(0x00, 0x33, 0x66, 0x00),   // 56
                           array(0x33, 0x99, 0x66, 0x00),   // 57
                           array(0x00, 0x33, 0x00, 0x00),   // 58
                           array(0x33, 0x33, 0x00, 0x00),   // 59
                           array(0x99, 0x33, 0x00, 0x00),   // 60
                           array(0x99, 0x33, 0x66, 0x00),   // 61
                           array(0x33, 0x33, 0x99, 0x00),   // 62
                           array(0x33, 0x33, 0x33, 0x00),   // 63
                         );
    }
    
    
    ###############################################################################
    #
    # _tmpfile_warning()
    #
    # Check that tmp files can be created for use in Worksheet.pm. A CGI, mod_perl
    # or IIS might not have permission to create tmp files. The test is here rather
    # than in Worksheet.pm so that only one warning is given.
    #
    /*sub _tmpfile_warning {
    
        my $fh = IO::File->new_tmpfile();
    
        if ((not defined $fh) && ($^W)) {
            carp("Unable to create tmp files via IO::File->new_tmpfile(). " .
                 "Storing data in memory")
    }
    }*/
    
    /**
    * Assemble worksheets into a workbook and send the BIFF data to an OLE
    * storage.
    */
    function store_workbook()
    {
        // Ensure that at least one worksheet has been selected.
        if ($this->activesheet == 0) {
            $this->worksheets[0]->selected = 1;
        }
    
        // Calculate the number of selected worksheet tabs and call the finalization
        // methods for each worksheet
        for($i=0; $i < count($this->worksheets); $i++)
        {
            if($this->worksheets[$i]->selected)
              $this->selected++;
            $this->worksheets[$i]->close($this->sheetnames);
        }
    
        // Add Workbook globals
        $this->_store_bof(0x0005);
        $this->_store_externs();    // For print area and repeat rows
        $this->_store_names();      // For print area and repeat rows
        $this->_store_window1();
        $this->_store_1904();
        $this->_store_all_fonts();
        $this->_store_all_num_formats();
        $this->_store_all_xfs();
        $this->_store_all_styles();
        $this->_store_palette();
        $this->_calc_sheet_offsets();
    
        // Add BOUNDSHEET records
        for($i=0; $i < count($this->worksheets); $i++) {
            $this->_store_boundsheet($this->worksheets[$i]->name,$this->worksheets[$i]->offset);
        }
    
        // End Workbook globals
        $this->_store_eof();

        // Store the workbook in an OLE container
        $this->_store_OLE_file();
    }
    
    /**
    * Store the workbook in an OLE container if the total size of the workbook data
    * is less than ~ 7MB.
    */
    function _store_OLE_file()
    {
        $OLE  = new OLEwriter($this->_filename);
        // Write Worksheet data if data <~ 7MB
        if ($OLE->set_size($this->_biffsize))
        {
            $OLE->write_header();
            $OLE->write($this->_data);
            foreach($this->worksheets as $sheet) 
            {
                while ($tmp = $sheet->get_data()) {
                    $OLE->write($tmp);
                }
            }
        }
        $OLE->close();
    }
    
    /**
    * Calculate offsets for Worksheet BOF records.
    */
    function _calc_sheet_offsets()
    {
        $BOF     = 11;
        $EOF     = 4;
        $offset  = $this->_datasize;
        for($i=0; $i < count($this->worksheets); $i++) {
            $offset += $BOF + strlen($this->worksheets[$i]->name);
        }
        $offset += $EOF;
        for($i=0; $i < count($this->worksheets); $i++) {
            $this->worksheets[$i]->offset = $offset;
            $offset += $this->worksheets[$i]->_datasize;
        }
        $this->_biffsize = $offset;
    }
    
    /**
    * Store the Excel FONT records.
    */
    function _store_all_fonts()
    {
        // tmp_format is added by new(). We use this to write the default XF's
        $format = $this->tmp_format;
        $font   = $format->get_font();
    
        // Note: Fonts are 0-indexed. According to the SDK there is no index 4,
        // so the following fonts are 0, 1, 2, 3, 5
        //
        for($i=1; $i <= 5; $i++){
            $this->_append($font);
        }
    
        // Iterate through the XF objects and write a FONT record if it isn't the
        // same as the default FONT and if it hasn't already been used.
        //
        $fonts = array();
        $index = 6;                  // The first user defined FONT
    
        $key = $format->get_font_key(); // The default font from _tmp_format
        $fonts[$key] = 0;               // Index of the default font
    
        for($i=0; $i < count($this->formats); $i++) {
            $key = $this->formats[$i]->get_font_key();
            if (isset($fonts[$key])) {
                // FONT has already been used
                $this->formats[$i]->font_index = $fonts[$key];
            }
            else {
                // Add a new FONT record
                $fonts[$key]        = $index;
                $this->formats[$i]->font_index = $index;
                $index++;
                $font = $this->formats[$i]->get_font();
                $this->_append($font);
            }
        }
    }
    
    /**
    * Store user defined numerical formats i.e. FORMAT records
    */
    function _store_all_num_formats()
    {
        // Leaning num_format syndrome
        $hash_num_formats = array();
        $num_formats      = array();
        $index = 164;
    
        // Iterate through the XF objects and write a FORMAT record if it isn't a
        // built-in format type and if the FORMAT string hasn't already been used.
        //
        for($i=0; $i < count($this->formats); $i++)
        {
            $num_format = $this->formats[$i]->_num_format;
    
            // Check if $num_format is an index to a built-in format.
            // Also check for a string of zeros, which is a valid format string
            // but would evaluate to zero.
            //
            if (!preg_match("/^0+\d/",$num_format))
            {
                if (preg_match("/^\d+$/",$num_format)) { // built-in format
                    continue;
                }
            }
    
            if (isset($hash_num_formats[$num_format])) {
                // FORMAT has already been used
                $this->formats[$i]->_num_format = $hash_num_formats[$num_format];
            }
            else
            {
                // Add a new FORMAT
                $hash_num_formats[$num_format]  = $index;
                $this->formats[$i]->_num_format = $index;
                array_push($num_formats,$num_format);
                $index++;
            }
        }
    
        // Write the new FORMAT records starting from 0xA4
        $index = 164;
        foreach ($num_formats as $num_format)
        {
            $this->_store_num_format($num_format,$index);
            $index++;
        }
    }
    
    /**
    * Write all XF records.
    */
    function _store_all_xfs()
    {
        // tmp_format is added by the constructor. We use this to write the default XF's
        // The default font index is 0
        //
        $format = $this->tmp_format;
        for ($i=0; $i <= 14; $i++)
        {
            $xf = $format->get_xf('style'); // Style XF
            $this->_append($xf);
        }
    
        $xf = $format->get_xf('cell');      // Cell XF
        $this->_append($xf);
    
        // User defined XFs
        for($i=0; $i < count($this->formats); $i++)
        {
            $xf = $this->formats[$i]->get_xf('cell');
            $this->_append($xf);
        }
    }
    
    /**
    * Write all STYLE records.
    */
    function _store_all_styles()
    {
        $this->_store_style();
    }
    
    /**
    * Write the EXTERNCOUNT and EXTERNSHEET records. These are used as indexes for
    * the NAME records.
    */
    function _store_externs()
    {
        // Create EXTERNCOUNT with number of worksheets
        $this->_store_externcount(count($this->worksheets));
    
        // Create EXTERNSHEET for each worksheet
        foreach ($this->sheetnames as $sheetname) {
            $this->_store_externsheet($sheetname);
        }
    }
    
    /**
    * Write the NAME record to define the print area and the repeat rows and cols.
    */
    function _store_names()
    {
        // Create the print area NAME records
        foreach ($this->worksheets as $worksheet)
        {
            // Write a Name record if the print area has been defined
            if (isset($worksheet->_print_rowmin))
            {
                $this->store_name_short(
                    $worksheet->index,
                    0x06, // NAME type
                    $worksheet->_print_rowmin,
                    $worksheet->_print_rowmax,
                    $worksheet->_print_colmin,
                    $worksheet->_print_colmax
                    );
            }
        }
    
        // Create the print title NAME records
        foreach ($this->worksheets as $worksheet)
        {
            $rowmin = $worksheet->_title_rowmin;
            $rowmax = $worksheet->_title_rowmax;
            $colmin = $worksheet->_title_colmin;
            $colmax = $worksheet->_title_colmax;
    
            // Determine if row + col, row, col or nothing has been defined
            // and write the appropriate record
            //
            if (isset($rowmin) && isset($colmin))
            {
                // Row and column titles have been defined.
                // Row title has been defined.
                $this->store_name_long(
                    $worksheet->index,
                    0x07, // NAME type
                    $rowmin,
                    $rowmax,
                    $colmin,
                    $colmax
                    );
            }
            elseif (isset($rowmin))
            {
                // Row title has been defined.
                $this->store_name_short(
                    $worksheet->index,
                    0x07, // NAME type
                    $rowmin,
                    $rowmax,
                    0x00,
                    0xff
                    );
            }
            elseif (isset($colmin))
            {
                // Column title has been defined.
                $this->store_name_short(
                    $worksheet->index,
                    0x07, // NAME type
                    0x0000,
                    0x3fff,
                    $colmin,
                    $colmax
                    );
            }
            else {
                // Print title hasn't been defined.
            }
        }
    }
    

    
    
    /******************************************************************************
    *
    * BIFF RECORDS
    *
    */
    
    /**
    * Write Excel BIFF WINDOW1 record.
    */
    function _store_window1()
    {
        $record    = 0x003D;                 // Record identifier
        $length    = 0x0012;                 // Number of bytes to follow
    
        $xWn       = 0x0000;                 // Horizontal position of window
        $yWn       = 0x0000;                 // Vertical position of window
        $dxWn      = 0x25BC;                 // Width of window
        $dyWn      = 0x1572;                 // Height of window
    
        $grbit     = 0x0038;                 // Option flags
        $ctabsel   = $this->selected;        // Number of workbook tabs selected
        $wTabRatio = 0x0258;                 // Tab to scrollbar ratio
    
        $itabFirst = $this->firstsheet;   // 1st displayed worksheet
        $itabCur   = $this->activesheet;  // Active worksheet
    
        $header    = pack("vv",        $record, $length);
        $data      = pack("vvvvvvvvv", $xWn, $yWn, $dxWn, $dyWn,
                                       $grbit,
                                       $itabCur, $itabFirst,
                                       $ctabsel, $wTabRatio);
        $this->_append($header.$data);
    }
    
    /**
    * Writes Excel BIFF BOUNDSHEET record.
    *
    * @param string  $sheetname Worksheet name
    * @param integer $offset    Location of worksheet BOF
    */
    function _store_boundsheet($sheetname,$offset)
    {
        $record    = 0x0085;                    // Record identifier
        $length    = 0x07 + strlen($sheetname); // Number of bytes to follow
    
        $grbit     = 0x0000;                    // Sheet identifier
        $cch       = strlen($sheetname);        // Length of sheet name
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("VvC", $offset, $grbit, $cch);
        $this->_append($header.$data.$sheetname);
    }
    
    /**
    * Write Excel BIFF STYLE records.
    */
    function _store_style()
    {
        $record    = 0x0293;   // Record identifier
        $length    = 0x0004;   // Bytes to follow
                               
        $ixfe      = 0x8000;   // Index to style XF
        $BuiltIn   = 0x00;     // Built-in style
        $iLevel    = 0xff;     // Outline style level
    
        $header    = pack("vv",  $record, $length);
        $data      = pack("vCC", $ixfe, $BuiltIn, $iLevel);
        $this->_append($header.$data);
    }
    
    
    /**
    * Writes Excel FORMAT record for non "built-in" numerical formats.
    *
    * @param string  $format Custom format string
    * @param integer $ifmt   Format index code
    */
    function _store_num_format($format,$ifmt)
    {
        $record    = 0x041E;                      // Record identifier
        $length    = 0x03 + strlen($format);      // Number of bytes to follow
    
        $cch       = strlen($format);             // Length of format string
    
        $header    = pack("vv", $record, $length);
        $data      = pack("vC", $ifmt, $cch);
        $this->_append($header.$data.$format);
    }
    
    /**
    * Write Excel 1904 record to indicate the date system in use.
    */
    function _store_1904()
    {
        $record    = 0x0022;         // Record identifier
        $length    = 0x0002;         // Bytes to follow

        $f1904     = $this->_1904;   // Flag for 1904 date system
    
        $header    = pack("vv", $record, $length);
        $data      = pack("v", $f1904);
        $this->_append($header.$data);
    }
    
    
    /**
    * Write BIFF record EXTERNCOUNT to indicate the number of external sheet
    * references in the workbook.
    *
    * Excel only stores references to external sheets that are used in NAME.
    * The workbook NAME record is required to define the print area and the repeat
    * rows and columns.
    *
    * A similar method is used in Worksheet.php for a slightly different purpose.
    *
    * @param integer $cxals Number of external references
    */
    function _store_externcount($cxals)
    {
        $record   = 0x0016;          // Record identifier
        $length   = 0x0002;          // Number of bytes to follow
    
        $header   = pack("vv", $record, $length);
        $data     = pack("v",  $cxals);
        $this->_append($header.$data);
    }
    
    
    /**
    * Writes the Excel BIFF EXTERNSHEET record. These references are used by
    * formulas. NAME record is required to define the print area and the repeat
    * rows and columns.
    *
    * A similar method is used in Worksheet.php for a slightly different purpose.
    *
    * @param string $sheetname Worksheet name
    */
    function _store_externsheet($sheetname)
    {
        $record      = 0x0017;                     // Record identifier
        $length      = 0x02 + strlen($sheetname);  // Number of bytes to follow
                                                   
        $cch         = strlen($sheetname);         // Length of sheet name
        $rgch        = 0x03;                       // Filename encoding
    
        $header      = pack("vv",  $record, $length);
        $data        = pack("CC", $cch, $rgch);
        $this->_append($header.$data.$sheetname);
    }
    
    
    /**
    * Store the NAME record in the short format that is used for storing the print
    * area, repeat rows only and repeat columns only.
    *
    * @param integer $index  Sheet index
    * @param integer $type   Built-in name type
    * @param integer $rowmin Start row
    * @param integer $rowmax End row
    * @param integer $colmin Start colum
    * @param integer $colmax End column
    */
    function store_name_short($index,$type,$rowmin,$rowmax,$colmin,$colmax)
    {
        $record          = 0x0018;       // Record identifier
        $length          = 0x0024;       // Number of bytes to follow
    
        $grbit           = 0x0020;       // Option flags
        $chKey           = 0x00;         // Keyboard shortcut
        $cch             = 0x01;         // Length of text name
        $cce             = 0x0015;       // Length of text definition
        $ixals           = $index + 1;   // Sheet index
        $itab            = $ixals;       // Equal to ixals
        $cchCustMenu     = 0x00;         // Length of cust menu text
        $cchDescription  = 0x00;         // Length of description text
        $cchHelptopic    = 0x00;         // Length of help topic text
        $cchStatustext   = 0x00;         // Length of status bar text
        $rgch            = $type;        // Built-in name type
    
        $unknown03       = 0x3b;
        $unknown04       = 0xffff-$index;
        $unknown05       = 0x0000;
        $unknown06       = 0x0000;
        $unknown07       = 0x1087;
        $unknown08       = 0x8005;
    
        $header             = pack("vv", $record, $length);
        $data               = pack("v", $grbit);
        $data              .= pack("C", $chKey);
        $data              .= pack("C", $cch);
        $data              .= pack("v", $cce);
        $data              .= pack("v", $ixals);
        $data              .= pack("v", $itab);
        $data              .= pack("C", $cchCustMenu);
        $data              .= pack("C", $cchDescription);
        $data              .= pack("C", $cchHelptopic);
        $data              .= pack("C", $cchStatustext);
        $data              .= pack("C", $rgch);
        $data              .= pack("C", $unknown03);
        $data              .= pack("v", $unknown04);
        $data              .= pack("v", $unknown05);
        $data              .= pack("v", $unknown06);
        $data              .= pack("v", $unknown07);
        $data              .= pack("v", $unknown08);
        $data              .= pack("v", $index);
        $data              .= pack("v", $index);
        $data              .= pack("v", $rowmin);
        $data              .= pack("v", $rowmax);
        $data              .= pack("C", $colmin);
        $data              .= pack("C", $colmax);
        $this->_append($header.$data);
    }
    
    
    /**
    * Store the NAME record in the long format that is used for storing the repeat
    * rows and columns when both are specified. This share a lot of code with
    * _store_name_short() but we use a separate method to keep the code clean.
    * Code abstraction for reuse can be carried too far, and I should know. ;-)
    *
    * @param integer $index Sheet index
    * @param integer $type  Built-in name type
    * @param integer $rowmin Start row
    * @param integer $rowmax End row
    * @param integer $colmin Start colum
    * @param integer $colmax End column
    */
    function store_name_long($index,$type,$rowmin,$rowmax,$colmin,$colmax)
    {
        $record          = 0x0018;       // Record identifier
        $length          = 0x003d;       // Number of bytes to follow
        $grbit           = 0x0020;       // Option flags
        $chKey           = 0x00;         // Keyboard shortcut
        $cch             = 0x01;         // Length of text name
        $cce             = 0x002e;       // Length of text definition
        $ixals           = $index + 1;   // Sheet index
        $itab            = $ixals;       // Equal to ixals
        $cchCustMenu     = 0x00;         // Length of cust menu text
        $cchDescription  = 0x00;         // Length of description text
        $cchHelptopic    = 0x00;         // Length of help topic text
        $cchStatustext   = 0x00;         // Length of status bar text
        $rgch            = $type;        // Built-in name type
    
        $unknown01       = 0x29;
        $unknown02       = 0x002b;
        $unknown03       = 0x3b;
        $unknown04       = 0xffff-$index;
        $unknown05       = 0x0000;
        $unknown06       = 0x0000;
        $unknown07       = 0x1087;
        $unknown08       = 0x8008;
    
        $header             = pack("vv",  $record, $length);
        $data               = pack("v", $grbit);
        $data              .= pack("C", $chKey);
        $data              .= pack("C", $cch);
        $data              .= pack("v", $cce);
        $data              .= pack("v", $ixals);
        $data              .= pack("v", $itab);
        $data              .= pack("C", $cchCustMenu);
        $data              .= pack("C", $cchDescription);
        $data              .= pack("C", $cchHelptopic);
        $data              .= pack("C", $cchStatustext);
        $data              .= pack("C", $rgch);
        $data              .= pack("C", $unknown01);
        $data              .= pack("v", $unknown02);
        // Column definition
        $data              .= pack("C", $unknown03);
        $data              .= pack("v", $unknown04);
        $data              .= pack("v", $unknown05);
        $data              .= pack("v", $unknown06);
        $data              .= pack("v", $unknown07);
        $data              .= pack("v", $unknown08);
        $data              .= pack("v", $index);
        $data              .= pack("v", $index);
        $data              .= pack("v", 0x0000);
        $data              .= pack("v", 0x3fff);
        $data              .= pack("C", $colmin);
        $data              .= pack("C", $colmax);
        // Row definition
        $data              .= pack("C", $unknown03);
        $data              .= pack("v", $unknown04);
        $data              .= pack("v", $unknown05);
        $data              .= pack("v", $unknown06);
        $data              .= pack("v", $unknown07);
        $data              .= pack("v", $unknown08);
        $data              .= pack("v", $index);
        $data              .= pack("v", $index);
        $data              .= pack("v", $rowmin);
        $data              .= pack("v", $rowmax);
        $data              .= pack("C", 0x00);
        $data              .= pack("C", 0xff);
        // End of data
        $data              .= pack("C", 0x10);
        $this->_append($header.$data);
    }
    
    
    /**
    * Stores the PALETTE biff record.
    */
    function _store_palette()
    {
        $aref            = $this->palette;
    
        $record          = 0x0092;                 // Record identifier
        $length          = 2 + 4 * count($aref);   // Number of bytes to follow
        $ccv             =         count($aref);   // Number of RGB values to follow
        $data = '';                                // The RGB data
    
        // Pack the RGB data
        foreach($aref as $color)
        {
            foreach($color as $byte) {
                $data .= pack("C",$byte);
            }
        }
    
        $header = pack("vvv",  $record, $length, $ccv);
        $this->_append($header.$data);
    }
}
?>