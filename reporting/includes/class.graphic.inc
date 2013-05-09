<?php
/**
*
*   graph
*   version 1.0
*
*
*
* Author: Carlos Reche/Joe Hunt
* E-mail: carlosreche@yahoo.com/joe.hunt.consulting@gmail.com
* Sorocaba, SP - Brazil/Wellington, New Zealand
*
* Created: Sep 20, 2004
* Last Modification: Sep 20, 2004/Apr 01, 2007
*
*
*
*  Authors' comments:
*
*  graph creates 6 different types of graphics with how many parameters you want. You can
*  change the appearance of the graphics in 3 different skins, and you can still cross data from 2 
*  graphics in only 1! It's a powerful script, and I recommend you read all the instructions 
*  to learn how to use all of this features. Don't worry, it's very simple to use it.
*
*  This script is free. Please keep the credits.
*
*/

/**

   INSTRUNCTIONS OF HOW TO USE THIS SCRIPT  (Please, take a minute to read it. It's important!)


   NOTE: make sure that your PHP is compiled to work with GD Lib.

///// START OF EXAMPLE.PHP /////

<?php

require "class.graphics.inc";
$pg = new graph;

$pg->title = "Sex";
$pg->type  = "5";
$pg->x[0]  = "male";
$pg->y[0]  = "50";
$pg->x[1]  = "female";
$pg->y[1]  = "55";
$pg->display();
?>

In your html file you set it up as:
.....
<img src="example.php" border="1" />
.....

You can supply extra parameters to display(). Ex. $pg->display("test.png") will save the image to a file.
Ex. $pg->display("", true) will paint a border around the image. It might be suitable if you choose to save to
file for later presentation.

///// END OF EXAMPLE.PHP /////



   Here is a list of all parameters you may set:

   title      =>  Title of the graphic
   axis_x     =>  Name of values from Axis X
   axis_y     =>  Name of values from Axis Y
   graphic_1  =>  Name of Graphic_1 (only shown if you are gonna cross data from 2 different graphics)
   graphic_2  =>  Name of Graphic_2 (same comment of above)

   type  =>  Type of graphic (values 1 to 6)
                1 => Vertical bars (default)
                2 => Horizontal bars
                3 => Dots
                4 => Lines
                5 => Pie
                6 => Donut

   skin   => Skin of the graphic (values 1 to 3)
                1 => Office (default)
                2 => Matrix
                3 => Spring

   credits => Only if you want to show my credits in the image. :)
                0 => doesn't show (default)
                1 => shows

   x[0]  =>  Name of the first parameter in Axis X
   x[1]  =>  Name of the second parameter in Axis X
   ... (etc)

   y[0]  =>  Value from "graphic_1" relative for "x[0]"
   y[1]  =>  Value from "graphic_1" relative for "x[1]"
   ... (etc)

   z[0]  =>  Value from "graphic_2" relative for "x[0]"
   z[1]  =>  Value from "graphic_2" relative for "x[1]"
   ... (etc)


   NOTE: You can't cross data between graphics if you use "pie" or "donut" graphic. Values for "z"
   won't be considerated.

   That's all! Hope you make a good use of it!
   It would be nice to receive feedback from others users. All comments are welcome!

   Regards,

   Carlos Reche

*/

class graph 
{

    var $x;
    var $y;
    var $z;

    var $title;
    var $axis_x;
    var $axis_y;
    var $graphic_1;
    var $graphic_2;
    var $type = 1;
    var $skin = 1;
    var $credits = 0;
    var $latin_notation;

    var $width;
    var $height;
    var $height_title;
    var $alternate_x;

	var $size = 2;
	var $tsize = 5;

    var $total_parameters;
    var $sum_total;
    var $biggest_value;
    var $biggest_parameter;
    var $available_types;
    var $dec1 = 0;
    var $dec2 = 0;
    var $h3d = 15; // 3D height
	var $built_in = true;
	var $fontfile = "";
	var $encoding;
	
    function graph()
    {
		$this->encoding = $_SESSION['language']->encoding;
		$path = dirname(__FILE__).'/../fonts/';

		// If you use utf-8 encoding you have to download and install FreeSans.ttf font.
		// It is not bundled with application due to its size.
		$this->fontfile = $this->encoding=='UTF-8' ? $path.'FreeSans.ttf' : $path.'LiberationSans-Regular.ttf';

        $this->x = $this->y = $this->z = array();
        $this->biggest_x        = NULL;
        $this->biggest_y        = NULL;
        $this->alternate_x      = false;
        $this->graphic_2_exists = false;
        $this->total_parameters = 0;
        $this->sum_total        = 1;
		$this->latin_notation   = false;
    }


    function display($save="", $border=false)
    {
        $this->legend_exists        = (preg_match("/(5|6)/", $this->type)) ? true : false;
        $this->biggest_graphic_name = (strlen($this->graphic_1) > strlen($this->graphic_2)) ? $this->graphic_1 : $this->graphic_2;
        $this->height_title         = (!empty($this->title)) ? ($this->string_height($this->tsize) + 15) : 0;
        $this->space_between_bars   = ($this->type == 1) ? 40 : 30;
        $this->space_between_dots   = 40;
        $this->higher_value         = 0;
        $this->higher_value_str     = 0;

        $this->width               = 0;
        $this->height              = 0;
        $this->graphic_area_width  = 0;
        $this->graphic_area_height = 0;
        $this->graphic_area_x1     = 30;
        $this->graphic_area_y1     = 20 + $this->height_title;
        $this->graphic_area_x2     = $this->graphic_area_x1 + $this->graphic_area_width;
        $this->graphic_area_y2     = $this->graphic_area_y1 + $this->graphic_area_height;

		if (count($this->z) && (preg_match("/(1|2|3|4)/", $this->type)))
            $this->graphic_2_exists = true;
        $this->total_parameters    = count($this->x);
		for ($i = 0; $i < $this->total_parameters; $i++)
		{
			if (strlen($this->x[$i]) > strlen($this->biggest_x))
                $this->biggest_x = $this->x[$i];
			if ($this->y[$i] > $this->biggest_y)
                $this->biggest_y = number_format(round($this->y[$i], 1), 1, ".", "");
            if ($this->graphic_2_exists)    
            {
            	if (isset($this->z[$i]) && $this->z[$i] > $this->biggest_y)
                	$this->biggest_y = number_format(round($this->z[$i], 1), 1, ".", "");
            }		
        }

        if (($this->graphic_2_exists == true)  &&  ((!empty($this->graphic_1)) || (!empty($this->graphic_2))))
        {
            $this->legend_exists = true;
        }

        $this->sum_total           = array_sum($this->y);
        if ($this->sum_total == 0)
        	$this->sum_total = 1;
        $this->space_between_bars += ($this->graphic_2_exists == true) ? 10 : 0;

        $this->calculate_higher_value();
        $this->calculate_width();
        $this->calculate_height();

        $this->create_graphic($save, $border);
    }

    function create_graphic($save="", $border=false)
    {
    	$size = 3;
        $this->img = imagecreatetruecolor($this->width, $this->height);

        $this->load_color_palette();

        // Fill background
        imagefill($this->img, 0, 0, $this->color['background']);
        //imagefilledrectangle($this->img, 0, 0, $this->width, $this->height, $this->color['background']);
        //if ($border)
        //	imagerectangle($this->img, 0, 0, $this->width-1, $this->height-1, imagecolorallocate($this->img, 100, 150, 200));

        // Draw title
        if (!empty($this->title))
        {
            $center = ($this->width / 2) - ($this->string_width($this->title, $this->tsize) / 2);
            $this->_imagestring($this->img, $this->tsize, $center, 10, $this->title, $this->color['title']);
        }


        // Draw axis and background lines for "vertical bars", "dots" and "lines"
        if (preg_match("/^(1|3|4)$/", $this->type))
        {
            if ($this->legend_exists == true)
            {
                $this->draw_legend();
            }

            $higher_value_y    = $this->graphic_area_y1 + (0.1 * $this->graphic_area_height);
            $higher_value_size = 0.9 * $this->graphic_area_height;

            $less  = 7 * strlen($this->higher_value_str);

            imageline($this->img, $this->graphic_area_x1, $higher_value_y, $this->graphic_area_x2, $higher_value_y, $this->color['bg_lines']);
            $this->_imagestring($this->img, $this->size, ($this->graphic_area_x1-$less-7), ($higher_value_y-7), $this->higher_value_str, $this->color['axis_values']);

            for ($i = 1; $i < 10; $i++)
            {
                $dec_y = $i * ($higher_value_size / 10);
                $x1 = $this->graphic_area_x1;
                $y1 = $this->graphic_area_y2 - $dec_y;
                $x2 = $this->graphic_area_x2;
                $y2 = $this->graphic_area_y2 - $dec_y;

                imageline($this->img, $x1, $y1, $x2, $y2, $this->color['bg_lines']);
                if ($i % 2 == 0) 
                {
                    $value = $this->number_formated($this->higher_value * $i / 10, $this->dec1);
                    $less = 7 * strlen($value);
                    $this->_imagestring($this->img, $this->size, ($x1-$less-7), ($y2-7), $value, $this->color['axis_values']);
                }
            }

            // Axis X
            $this->_imagestring($this->img, $this->size, $this->graphic_area_x2+10, $this->graphic_area_y2+3, $this->axis_x, $this->color['title']);
            imageline($this->img, $this->graphic_area_x1, $this->graphic_area_y2, $this->graphic_area_x2, $this->graphic_area_y2, $this->color['axis_line']);
            // Axis Y
            $this->_imagestring($this->img, $this->size, 20, $this->graphic_area_y1-20, $this->axis_y, $this->color['title']);
            imageline($this->img, $this->graphic_area_x1, $this->graphic_area_y1, $this->graphic_area_x1, $this->graphic_area_y2, $this->color['axis_line']);
        }


        // Draw axis and background lines for "horizontal bars"
        else if ($this->type == 2)
        {
            if ($this->legend_exists == true)
            {
                $this->draw_legend();
            }

            $higher_value_x    = $this->graphic_area_x2 - (0.2 * $this->graphic_area_width);
            $higher_value_size = 0.8 * $this->graphic_area_width;

            imageline($this->img, ($this->graphic_area_x1+$higher_value_size), $this->graphic_area_y1, ($this->graphic_area_x1+$higher_value_size), $this->graphic_area_y2, $this->color['bg_lines']);
            $this->_imagestring($this->img, $this->size, (($this->graphic_area_x1+$higher_value_size) - ($this->string_width($this->higher_value, $this->size)/2)), ($this->graphic_area_y2+2), $this->higher_value_str, $this->color['axis_values']);

            for ($i = 1, $alt = 15; $i < 10; $i++)
            {
                $dec_x = number_format(round($i * ($higher_value_size  / 10), 1), 1, ".", "");

                imageline($this->img, ($this->graphic_area_x1+$dec_x), $this->graphic_area_y1, ($this->graphic_area_x1+$dec_x), $this->graphic_area_y2, $this->color['bg_lines']);
                if ($i % 2 == 0) 
                {
                    $alt   = (strlen($this->biggest_y) > 4 && $alt != 15) ? 15 : 2;
                    $value = $this->number_formated($this->higher_value * $i / 10, $this->dec1);
                    $this->_imagestring($this->img, $this->size, (($this->graphic_area_x1+$dec_x) - ($this->string_width($this->higher_value, $this->size)/2)), ($this->graphic_area_y2), $value, $this->color['axis_values'], $alt);
                }
            }

            // Axis X
            $this->_imagestring($this->img, $this->size, ($this->graphic_area_x2+10), ($this->graphic_area_y2+3), $this->axis_y, $this->color['title']);
            imageline($this->img, $this->graphic_area_x1, $this->graphic_area_y2, $this->graphic_area_x2, $this->graphic_area_y2, $this->color['axis_line']);
            // Axis Y
            $this->_imagestring($this->img, $this->size, 20, ($this->graphic_area_y1-20), $this->axis_x, $this->color['title']);
            imageline($this->img, $this->graphic_area_x1, $this->graphic_area_y1, $this->graphic_area_x1, $this->graphic_area_y2, $this->color['axis_line']);
        }


        // Draw legend box for "pie" or "donut"
        else if (preg_match("/^(5|6)$/", $this->type))
        {
            $this->draw_legend();
        }



        /**
        * Draw graphic: VERTICAL BARS
        */
        if ($this->type == 1)
        {
            $num = 1;
            $x   = $this->graphic_area_x1 + 20;

            foreach ($this->x as $i => $parameter)
            {
                if (isset($this->z[$i])) 
                {
                    $size = round($this->z[$i] * $higher_value_size / $this->higher_value);
                    $x1   = $x + 10;
                    $y1   = ($this->graphic_area_y2 - $size) + 1;
                    $x2   = $x1 + 20;
                    $y2   = $this->graphic_area_y2 - 1;
                    imageline($this->img, ($x1+1), ($y1-1), $x2, ($y1-1), $this->color['bars_2_shadow']);
                    imageline($this->img, ($x2+1), ($y1-1), ($x2+1), $y2, $this->color['bars_2_shadow']);
                    imageline($this->img, ($x2+2), ($y1-1), ($x2+2), $y2, $this->color['bars_2_shadow']);
                    imagefilledrectangle($this->img, $x1, $y1, $x2, $y2, $this->color['bars_2']);
                }

                $size = round($this->y[$i] * $higher_value_size / $this->higher_value);
                $alt  = (($num % 2 == 0) && (strlen($this->biggest_x) > 5)) ? 15 : 2;
                $x1   = $x;
                $y1   = ($this->graphic_area_y2 - $size) + 1;
                $x2   = $x1 + 20;
                $y2   = $this->graphic_area_y2 - 1;
                $x   += $this->space_between_bars;
                $num++;

                imageline($this->img, ($x1+1), ($y1-1), $x2, ($y1-1), $this->color['bars_shadow']);
                imageline($this->img, ($x2+1), ($y1-1), ($x2+1), $y2, $this->color['bars_shadow']);
                imageline($this->img, ($x2+2), ($y1-1), ($x2+2), $y2, $this->color['bars_shadow']);
                imagefilledrectangle($this->img, $x1, $y1, $x2, $y2, $this->color['bars']);
                $this->_imagestring($this->img, $this->size, ((($x1+$x2)/2) - (strlen($parameter)*7/2)), ($y2+2), $parameter, $this->color['axis_values'], $alt);
            }
        }


        /**
        * Draw graphic: HORIZONTAL BARS
        */
        else if ($this->type == 2)
        {
            $y = 10;

            foreach ($this->x as $i => $parameter)
            {
                if (isset($this->z[$i])) 
                {
                    $size = round($this->z[$i] * $higher_value_size / $this->higher_value);
                    $x1   = $this->graphic_area_x1 + 1;
                    $y1   = $this->graphic_area_y1 + $y + 10;
                    $x2   = $x1 + $size;
                    $y2   = $y1 + 15;
                    imageline($this->img, ($x1), ($y2+1), $x2, ($y2+1), $this->color['bars_2_shadow']);
                    imageline($this->img, ($x1), ($y2+2), $x2, ($y2+2), $this->color['bars_2_shadow']);
                    imageline($this->img, ($x2+1), ($y1+1), ($x2+1), ($y2+2), $this->color['bars_2_shadow']);
                    imagefilledrectangle($this->img, $x1, $y1, $x2, $y2, $this->color['bars_2']);
                    $this->_imagestring($this->img, $this->size, ($x2+7), ($y1+7), $this->number_formated($this->z[$i], $this->dec2), $this->color['bars_2_shadow']);
                }

                $size = round(($this->y[$i] / $this->higher_value) * $higher_value_size);
                $x1   = $this->graphic_area_x1 + 1;
                $y1   = $this->graphic_area_y1 + $y;
                $x2   = $x1 + $size;
                $y2   = $y1 + 15;
                $y   += $this->space_between_bars;

                imageline($this->img, ($x1), ($y2+1), $x2, ($y2+1), $this->color['bars_shadow']);
                imageline($this->img, ($x1), ($y2+2), $x2, ($y2+2), $this->color['bars_shadow']);
                imageline($this->img, ($x2+1), ($y1+1), ($x2+1), ($y2+2), $this->color['bars_shadow']);
                imagefilledrectangle($this->img, $x1, $y1, $x2, $y2, $this->color['bars']);
                $this->_imagestring($this->img, $this->size, ($x2+7), ($y1+2), $this->number_formated($this->y[$i], $this->dec2), $this->color['bars_shadow']);

                //$this->_imagestring($this->img, $this->size, ($x1 - ((strlen($parameter)*7)+7)), ($y1+2), $parameter, $this->color['axis_values']);
                $this->_imagestring($this->img, $this->size, 30, ($y1+2), $parameter, $this->color['axis_values']);
            }
        }


        /**
        * Draw graphic: DOTS or LINE
        */
        else if (preg_match("/^(3|4)$/", $this->type))
        {

            $x[0] = $this->graphic_area_x1+1;

            foreach ($this->x as $i => $parameter)
            {
                if ($this->graphic_2_exists == true) 
                {
                    $size  = round($this->z[$i] * $higher_value_size / $this->higher_value);
                    $z[$i] = $this->graphic_area_y2 - $size;
                }

                $alt   = (($i % 2 == 0) && (strlen($this->biggest_x) > 5)) ? 15 : 2;
                $size  = round($this->y[$i] * $higher_value_size / $this->higher_value);
                $y[$i] = $this->graphic_area_y2 - $size;

                if ($i != 0) 
                {
                    imageline($this->img, $x[$i], ($this->graphic_area_y1+10), $x[$i], ($this->graphic_area_y2-1), $this->color['bg_lines']);
                }
                $this->_imagestring($this->img, $this->size, ($x[$i] - (strlen($parameter)*7/2 )), ($this->graphic_area_y2+2), $parameter, $this->color['axis_values'], $alt);

                $x[$i+1] = $x[$i] + 40;
            }

            foreach ($x as $i => $value_x)
            {
                if ($this->graphic_2_exists == true)
                {
                    if (isset($z[$i+1])) 
                    {
                        // Draw lines
                        if ($this->type == 4)
                        {
                            imageline($this->img, $x[$i], $z[$i], $x[$i+1], $z[$i+1], $this->color['line_2']);
                            imageline($this->img, $x[$i], ($z[$i]+1), $x[$i+1], ($z[$i+1]+1), $this->color['line_2']);
                        }
                        imagefilledrectangle($this->img, $x[$i]-1, $z[$i]-1, $x[$i]+2, $z[$i]+2, $this->color['line_2']);
                    }
                    else 
                    { // Draw last dot
                        imagefilledrectangle($this->img, $x[$i-1]-1, $z[$i-1]-1, $x[$i-1]+2, $z[$i-1]+2, $this->color['line_2']);
                    }
                }

                if (count($y) > 1)
                {
                    if (isset($y[$i+1])) 
                    {
                        // Draw lines
                        if ($this->type == 4)
                        {
                            imageline($this->img, $x[$i], $y[$i], $x[$i+1], $y[$i+1], $this->color['line']);
                            imageline($this->img, $x[$i], ($y[$i]+1), $x[$i+1], ($y[$i+1]+1), $this->color['line']);
                        }
                        imagefilledrectangle($this->img, $x[$i]-1, $y[$i]-1, $x[$i]+2, $y[$i]+2, $this->color['line']);
                    }
                    else 
                    { // Draw last dot
                        imagefilledrectangle($this->img, $x[$i-1]-1, $y[$i-1]-1, $x[$i-1]+2, $y[$i-1]+2, $this->color['line']);
                    }
                }

            }
        }


        /**
        * Draw graphic: PIE or DONUT
        */
        else if (preg_match("/^(5|6)$/", $this->type))
        {
            $center_x = ($this->graphic_area_x1 + $this->graphic_area_x2) / 2;
            $center_y = ($this->graphic_area_y1 + $this->graphic_area_y2) / 2;
            $width    = $this->graphic_area_width;
            $height   = $this->graphic_area_height;
            $start    = 0;
            $sizes    = array();

            foreach ($this->x as $i => $parameter)
            {
                $size    = $this->y[$i] * 360 / $this->sum_total;
				$size = round2($size, 0);
                $sizes[] = $size;
                $start  += $size;
            }
            $start = 270;

            // Draw PIE
            if ($this->type == 5)
            {
                // Draw shadow
                foreach ($sizes as $i => $size)
                {
                    $num_color = $i + 1;
                    while ($num_color > 7) 
                    {
                        $num_color -= 5;
                    }
                    $color = 'arc_' . $num_color . '_shadow';

                    for ($i = $this->h3d; $i >= 0; $i--)
                    {
                        //imagearc($this->img, $center_x, ($center_y+$i), $width, $height, $start, ($start+$size), $this->color[$color]);
                        if ($size >= 1)
                        	imagefilledarc($this->img, $center_x, ($center_y+$i), $width, $height, $start, ($start+$size), $this->color[$color], IMG_ARC_NOFILL);
                    }
                    $start += $size;
                }

                $start = 270;

                // Draw pieces
                foreach ($sizes as $i => $size)
                {
                    $num_color = $i + 1;
                    while ($num_color > 7) 
                    {
                        $num_color -= 5;
                    }
                    $color = 'arc_' . $num_color;
                    if ($size >= 1)
                    	imagefilledarc($this->img, $center_x, $center_y, ($width+2), ($height+2), $start, ($start+$size), $this->color[$color], IMG_ARC_EDGED);
                    $start += $size;
                }
            }

            // Draw DONUT
            else if ($this->type == 6)
            {
                foreach ($sizes as $i => $size)
                {
                    $num_color = $i + 1;
                    while ($num_color > 7) 
                    {
                        $num_color -= 5;
                    }
                    $color        = 'arc_' . $num_color;
                    $color_shadow = 'arc_' . $num_color . '_shadow';
                    if ($size >= 1)
                    	imagefilledarc($this->img, $center_x, $center_y, $width, $height, $start, ($start+$size), $this->color[$color], IMG_ARC_PIE);
                    $start += $size;
                }
                imagefilledarc($this->img, $center_x, $center_y, 100, 100, 0, 360, $this->color['background'], IMG_ARC_PIE);
                imagearc($this->img, $center_x, $center_y, 100, 100, 0, 360, $this->color['bg_legend']);
                imagearc($this->img, $center_x, $center_y, ($width+1), ($height+1), 0, 360, $this->color['bg_legend']);
            }
        }


        if ($this->credits == true) 
        {
            $this->draw_credits();
        }
		if ($save != "")
			imagepng($this->img, $save);
		else
		{
        	header('Content-type: image/png');
        	imagepng($this->img);
        }	
        imagedestroy($this->img);
    }

    function calculate_width()
    {
        switch ($this->type)
        {
            // Vertical bars
            case 1:
                $this->legend_box_width   = ($this->legend_exists == true) ? ($this->string_width($this->biggest_graphic_name, $this->tsize) + 25) : 0;
                $this->graphic_area_width = ($this->space_between_bars * $this->total_parameters) + 30;
                $this->graphic_area_x1   += $this->string_width(($this->higher_value_str), $this->size);
                $this->width += $this->graphic_area_x1 + 20;
                $this->width += ($this->legend_exists == true) ? 50 : ((7 * strlen($this->axis_x)) + 10);
                break;

            // Horizontal bars
            case 2:
                $this->legend_box_width   = ($this->legend_exists == true) ? ($this->string_width($this->biggest_graphic_name, $this->size) + 25) : 0;
                $this->graphic_area_width = ($this->string_width($this->higher_value_str, $this->size) > 50) ? (5 * ($this->string_width($this->higher_value_str, $this->size)) * 0.85) : 200;
                $this->graphic_area_x1 += 7 * strlen($this->biggest_x);
                $this->width += ($this->legend_exists == true) ? 60 : ((7 * strlen($this->axis_y)) + 30);
                $this->width += $this->graphic_area_x1;
                break;

            // Dots
            case 3:
                $this->legend_box_width   = ($this->legend_exists == true) ? ($this->string_width($this->biggest_graphic_name, $this->size) + 25) : 0;
                $this->graphic_area_width = ($this->space_between_dots * $this->total_parameters) - 10;
                $this->graphic_area_x1   += $this->string_width(($this->higher_value_str), $this->size);
                $this->width += $this->graphic_area_x1 + 20;
                $this->width += ($this->legend_exists == true) ? 40 : ((7 * strlen($this->axis_x)) + 30);
                break;

            // Lines
            case 4:
                $this->legend_box_width   = ($this->legend_exists == true) ? ($this->string_width($this->biggest_graphic_name, $this->size) + 25) : 0;
                $this->graphic_area_width = ($this->space_between_dots * $this->total_parameters) - 10;
                $this->graphic_area_x1   += $this->string_width(($this->higher_value_str), $this->size);
                $this->width += $this->graphic_area_x1 + 20;
                $this->width += ($this->legend_exists == true) ? 40 : ((7 * strlen($this->axis_x)) + 30);
                break;

            // Pie
            case 5:
                $this->legend_box_width   = $this->string_width($this->biggest_x, $this->size) + 85;
                $this->graphic_area_width = 200;
                $this->width += 90;
                break;

            // Donut
            case 6:
                $this->legend_box_width   = $this->string_width($this->biggest_x, $this->size) + 85;
                $this->graphic_area_width = 180;
                $this->width += 90;
                break;
        }

        $this->graphic_area_width = max($this->graphic_area_width, $this->string_width($this->title, $this->size));
        $this->width += $this->graphic_area_width;
        $this->width += $this->legend_box_width;


        $this->graphic_area_x2 = $this->graphic_area_x1 + $this->graphic_area_width;
        $this->legend_box_x1   = $this->graphic_area_x2 + 40;
        $this->legend_box_x2   = $this->legend_box_x1 + $this->legend_box_width;
    }

    function calculate_height()
    {
        switch ($this->type)
        {
            // Vertical bars
            case 1:
                $this->legend_box_height   = ($this->graphic_2_exists == true) ? 40 : 0;
                $this->graphic_area_height = 150;
                $this->height += 65;
                break;

            // Horizontal bars
            case 2:
                $this->legend_box_height   = ($this->graphic_2_exists == true) ? 40 : 0;
                $this->graphic_area_height = ($this->space_between_bars * $this->total_parameters) + 10;
                $this->height += 65;
                break;

            // Dots
            case 3:
                $this->legend_box_height   = ($this->graphic_2_exists == true) ? 40 : 0;
                $this->graphic_area_height = 150;
                $this->height += 65;
                break;

            // Lines
            case 4:
                $this->legend_box_height   = ($this->graphic_2_exists == true) ? 40 : 0;
                $this->graphic_area_height = 150;
                $this->height += 65;
                break;

            // Pie
            case 5:
                $this->legend_box_height   = (!empty($this->axis_x)) ? 30 : 5;
                $this->legend_box_height  += (14 * $this->total_parameters);
                $this->graphic_area_height = 150;
                $this->height += 50;
                break;

            // Donut
            case 6:
                $this->legend_box_height   = (!empty($this->axis_x)) ? 30 : 5;
                $this->legend_box_height  += (14 * $this->total_parameters);
                $this->graphic_area_height = 180;
                $this->height += 50;
                break;
        }

        $this->height += $this->height_title;
        $this->height += ($this->legend_box_height > $this->graphic_area_height) ? ($this->legend_box_height - $this->graphic_area_height) : 0;
        $this->height += $this->graphic_area_height;

        $this->graphic_area_y2 = $this->graphic_area_y1 + $this->graphic_area_height;
        $this->legend_box_y1   = $this->graphic_area_y1 + 10;
        $this->legend_box_y2   = $this->legend_box_y1 + $this->legend_box_height;
    }

    function draw_legend()
    {
        $x1 = $this->legend_box_x1;
        $y1 = $this->legend_box_y1;
        $x2 = $this->legend_box_x2;
        $y2 = $this->legend_box_y2;

        imagefilledrectangle($this->img, $x1, $y1, $x2, $y2, $this->color['bg_legend']);

        $x = $x1 + 5;
        $y = $y1 + 5;


        // Draw legend values for VERTICAL BARS, HORIZONTAL BARS, DOTS and LINES
        if (preg_match("/^(1|2|3|4)$/", $this->type))
        {
            $color_1 = (preg_match("/^(1|2)$/", $this->type)) ? $this->color['bars']   : $this->color['line'];
            $color_2 = (preg_match("/^(1|2)$/", $this->type)) ? $this->color['bars_2'] : $this->color['line_2'];

            imagefilledrectangle($this->img, $x, $y, ($x+10), ($y+10), $color_1);
            imagerectangle($this->img, $x, $y, ($x+10), ($y+10), $this->color['title']);
            $this->_imagestring($this->img, $this->size, ($x+15), ($y-2), $this->graphic_1, $this->color['axis_values']);
            $y += 20;
            imagefilledrectangle($this->img, $x, $y, ($x+10), ($y+10), $color_2);
            imagerectangle($this->img, $x, $y, ($x+10), ($y+10), $this->color['title']);
            $this->_imagestring($this->img, $this->size, ($x+15), ($y-2), $this->graphic_2, $this->color['axis_values']);
        }

        // Draw legend values for PIE or DONUT
        else if (preg_match("/^(5|6)$/", $this->type))
        {
            if (!empty($this->axis_x))
            {
                $this->_imagestring($this->img, $this->size, ((($x1+$x2)/2) - (strlen($this->axis_x)*7/2)), $y, 
                	$this->axis_x." (".$this->graphic_1.")", $this->color['title']);
                $y += 25;
            }

            $num = 1;

            foreach ($this->x as $i => $parameter)
            {
                while ($num > 7) 
                {
                    $num -= 5;
                }
                $color = 'arc_' . $num;

                $percent = number_format(round(($this->y[$i] * 100 / $this->sum_total), 2), 2, ".", "") . ' %';
                $less    = (strlen($percent) * 7);

                if ($num != 1) 
                {
                    imageline($this->img, ($x1+15), ($y-2), ($x2-5), ($y-2), $this->color['bg_lines']);
                }
                imagefilledrectangle($this->img, $x, $y, ($x+10), ($y+10), $this->color[$color]);
                imagerectangle($this->img, $x, $y, ($x+10), ($y+10), $this->color['title']);
                $this->_imagestring($this->img, $this->size, ($x+15), ($y-2), $parameter, $this->color['axis_values']);
                $this->_imagestring($this->img, $this->size, ($x2-$less), ($y-2), $percent, $this->color['axis_values']);
                $y += 14;
                $num++;
            }
        }
    }


    function string_width($string, $size) 
    {
        $single_width = $size + 4;
        return $single_width * strlen($string);
    }

    function string_height($size) 
    {
        if ($size <= 1) 
        {
            $height = 8;
        } 
        else if ($size <= 3) 
        {
            $height = 12;
        } 
        else if ($size >= 4) 
        {
            $height = 14;
        }
        return $height;
    }

    function calculate_higher_value() 
    {
        $digits   = strlen(round($this->biggest_y));
        $interval = pow(10, ($digits-1));
        $this->higher_value     = round(($this->biggest_y - ($this->biggest_y % $interval) + $interval), 1);
        $this->higher_value_str = $this->number_formated($this->higher_value, $this->dec1);
    }

    function number_formated($number, $dec_size = 1)
    {
        if ($this->latin_notation == true) 
            return number_format(round($number, $dec_size), $dec_size, ",", ".");
       	return number_format(round($number, $dec_size), $dec_size, ".", ",");
    }

    function number_float($number)
    {
        if ($this->latin_notation == true) 
            $number = str_replace(".", "", $number);
       	return (float)str_replace(",", "", $number);
    }


    function draw_credits() 
    {
        $this->_imagestring($this->img, $this->size - 2, ($this->width-120), ($this->height-10), "Powered by Carlos Reche", $this->color['title']);
    }


    function load_color_palette()
    {
        switch ($this->skin)
        {
            // Office
            case 1:
                //$this->color['title']       = imagecolorallocate($this->img,  50,  50,  50);
                $this->color['title']       = imagecolorallocate($this->img,  40,  70,  130);
                //$this->color['background']  = imagecolorallocate($this->img, 238, 255, 238);
                $this->color['background']  = imagecolorallocate($this->img, 255, 255, 255);
                $this->color['axis_values'] = imagecolorallocate($this->img,  50,  50,  50);
                $this->color['axis_line']   = imagecolorallocate($this->img, 100, 100, 100);
                $this->color['bg_lines']    = imagecolorallocate($this->img, 220, 220, 220);
                $this->color['bg_legend']   = imagecolorallocate($this->img, 255, 255, 255);

                if (preg_match("/^(1|2)$/", $this->type))
                {
                    $this->color['bars']          = imagecolorallocate($this->img, 100, 150, 200);
                    $this->color['bars_shadow']   = imagecolorallocate($this->img,  50, 100, 150);
                    $this->color['bars_2']        = imagecolorallocate($this->img, 200, 250, 150);
                    $this->color['bars_2_shadow'] = imagecolorallocate($this->img, 120, 170,  70);
                }
                else if (preg_match("/^(3|4)$/", $this->type))
                {
                    $this->color['line']   = imagecolorallocate($this->img, 100, 150, 200);
                    $this->color['line_2'] = imagecolorallocate($this->img, 230, 100, 100);
                }
                else if (preg_match("/^(5|6)$/", $this->type))
                {
                    $this->color['arc_1']        = imagecolorallocate($this->img, 255, 150,   0);
                    $this->color['arc_2']        = imagecolorallocate($this->img, 150,   0, 255);
                    $this->color['arc_3']        = imagecolorallocate($this->img,   0, 255, 255);
                    $this->color['arc_4']        = imagecolorallocate($this->img, 255,   0,   0);
                    $this->color['arc_5']        = imagecolorallocate($this->img,   0, 255,   0);
                    $this->color['arc_6']        = imagecolorallocate($this->img,   0,   0, 255);
                    $this->color['arc_7']        = imagecolorallocate($this->img, 255, 255,   0);
                    $this->color['arc_1_shadow'] = imagecolorallocate($this->img, 127,  75,   0);
                    $this->color['arc_2_shadow'] = imagecolorallocate($this->img,  75,   0, 127);
                    $this->color['arc_3_shadow'] = imagecolorallocate($this->img,   0, 127, 127);
                    $this->color['arc_4_shadow'] = imagecolorallocate($this->img, 127,   0,   0);
                    $this->color['arc_5_shadow'] = imagecolorallocate($this->img,   0, 127,   0);
                    $this->color['arc_6_shadow'] = imagecolorallocate($this->img,   0,   0, 127);
                    $this->color['arc_7_shadow'] = imagecolorallocate($this->img, 127, 127,   0);
                }
                break;

            // Matrix
            case 2:
                $this->color['title']       = imagecolorallocate($this->img, 255, 255, 255);
                $this->color['background']  = imagecolorallocate($this->img,   0,   0,   0);
                $this->color['axis_values'] = imagecolorallocate($this->img,   0, 230,   0);
                $this->color['axis_line']   = imagecolorallocate($this->img,   0, 200,   0);
                $this->color['bg_lines']    = imagecolorallocate($this->img, 100, 100, 100);
                $this->color['bg_legend']   = imagecolorallocate($this->img,  70,  70,  70);

                if (preg_match("/^(1|2)$/", $this->type))
                {
                    $this->color['bars']          = imagecolorallocate($this->img,  50, 200,  50);
                    $this->color['bars_shadow']   = imagecolorallocate($this->img,   0, 150,   0);
                    $this->color['bars_2']        = imagecolorallocate($this->img, 255, 255, 255);
                    $this->color['bars_2_shadow'] = imagecolorallocate($this->img, 220, 220, 220);
                }
                else if (preg_match("/^(3|4)$/", $this->type))
                {
                    $this->color['line']   = imagecolorallocate($this->img, 220, 220, 220);
                    $this->color['line_2'] = imagecolorallocate($this->img,   0, 180,   0);
                }
                else if (preg_match("/^(5|6)$/", $this->type))
                {
                    $this->color['arc_1']        = imagecolorallocate($this->img, 255, 255, 255);
                    $this->color['arc_2']        = imagecolorallocate($this->img, 200, 220, 200);
                    $this->color['arc_3']        = imagecolorallocate($this->img, 160, 200, 160);
                    $this->color['arc_4']        = imagecolorallocate($this->img, 135, 180, 135);
                    $this->color['arc_5']        = imagecolorallocate($this->img, 115, 160, 115);
                    $this->color['arc_6']        = imagecolorallocate($this->img, 100, 140, 100);
                    $this->color['arc_7']        = imagecolorallocate($this->img,  90, 120,  90);
                    $this->color['arc_1_shadow'] = imagecolorallocate($this->img, 127, 127, 127);
                    $this->color['arc_2_shadow'] = imagecolorallocate($this->img, 100, 110, 100);
                    $this->color['arc_3_shadow'] = imagecolorallocate($this->img,  80, 100,  80);
                    $this->color['arc_4_shadow'] = imagecolorallocate($this->img,  67,  90,  67);
                    $this->color['arc_5_shadow'] = imagecolorallocate($this->img,  57,  80,  57);
                    $this->color['arc_6_shadow'] = imagecolorallocate($this->img,  50,  70,  50);
                    $this->color['arc_7_shadow'] = imagecolorallocate($this->img,  45,  60,  45);
                }
                break;


            // Spring
            case 3:
                $this->color['title']       = imagecolorallocate($this->img, 250,  50,  50);
                //$this->color['background']  = imagecolorallocate($this->img, 250, 250, 220);
                $this->color['background']  = imagecolorallocate($this->img, 255, 255, 255);
                $this->color['axis_values'] = imagecolorallocate($this->img,  50, 150,  50);
                $this->color['axis_line']   = imagecolorallocate($this->img,  50, 100,  50);
                $this->color['bg_lines']    = imagecolorallocate($this->img, 200, 224, 180);
                //$this->color['bg_legend']   = imagecolorallocate($this->img, 230, 230, 200);
                $this->color['bg_legend']   = imagecolorallocate($this->img, 255, 255, 255);

                if (preg_match("/^(1|2)$/", $this->type))
                {
                    $this->color['bars']          = imagecolorallocate($this->img, 255, 170,  80);
                    $this->color['bars_shadow']   = imagecolorallocate($this->img, 200, 120,  30);
                    $this->color['bars_2']        = imagecolorallocate($this->img, 250, 230,  80);
                    $this->color['bars_2_shadow'] = imagecolorallocate($this->img, 180, 150,   0);
                }
                else if (preg_match("/^(3|4)$/", $this->type))
                {
                    $this->color['line']   = imagecolorallocate($this->img, 230, 100,   0);
                    $this->color['line_2'] = imagecolorallocate($this->img, 220, 200,  50);
                }
                else if (preg_match("/^(5|6)$/", $this->type))
                {
                    $this->color['arc_1']        = imagecolorallocate($this->img, 100, 150, 200);
                    $this->color['arc_2']        = imagecolorallocate($this->img, 200, 250, 150);
                    $this->color['arc_3']        = imagecolorallocate($this->img, 250, 200, 150);
                    $this->color['arc_4']        = imagecolorallocate($this->img, 250, 150, 150);
                    $this->color['arc_5']        = imagecolorallocate($this->img, 250, 250, 150);
                    $this->color['arc_6']        = imagecolorallocate($this->img, 230, 180, 250);
                    $this->color['arc_7']        = imagecolorallocate($this->img, 200, 200, 150);
                    $this->color['arc_1_shadow'] = imagecolorallocate($this->img,  50,  75, 100);
                    $this->color['arc_2_shadow'] = imagecolorallocate($this->img, 100, 125,  75);
                    $this->color['arc_3_shadow'] = imagecolorallocate($this->img, 125, 100,  75);
                    $this->color['arc_4_shadow'] = imagecolorallocate($this->img, 125,  75,  75);
                    $this->color['arc_5_shadow'] = imagecolorallocate($this->img, 125, 125,  75);
                    $this->color['arc_6_shadow'] = imagecolorallocate($this->img, 115,  90, 125);
                    $this->color['arc_7_shadow'] = imagecolorallocate($this->img, 100, 100,  75);
                }
                break;

        }
    }

    function _imagestring($img, $size, $x, $y, $string, $col, $alt=0)
    {
       	if ($alt && strlen($string) > 12)
       		$string = substr($string, 0, 12);

		if ($this->encoding != 'UTF-8') {
			if (function_exists('iconv'))
				$string = iconv($this->encoding, 'UTF-8', $string);
			else
				$string = mb_convert_encoding($string, 'UTF-8', $this->encoding);
		}

    	if ($this->built_in)
    	{
    		imagestring($img, $size, $x, $y + $alt, $string, $col);
    	}	
    	else	
    	{
    		if ($size == 1)
    			$size = 7;
    		else if ($size == 2)
    			$size = 8;
    		else if ($size == 3)
    			$size = 9;
    		else if ($size == 4)
    			$size = 11;
    		else
    			$size = 12;
    		$y += $size + 3;	
    		//if ($alt)
    		//	$angle = -15;
    		//else
   				$angle = 0;

    		imagettftext($img, $size, $angle, $x, $y + $alt, $col, $this->fontfile, $string);
    	}
    }	
}
?>