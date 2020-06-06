<?php
# Load corpus words
$corpus_text = file_get_contents("corpus");
$corpus_words = explode("\n", $corpus_text);
echo count($corpus_words) . " words loaded.\n";

# Sizes wanted
$sizes = array(10000);

# Generate 100 files for each test case
foreach($sizes as $size)
{
    for($i = 0; $i < 20; ++$i)
    {
        $file_text = "";
        $file_name = "test_" . $size . "_$i";
        echo "Generating $file_name...\n";
        for($word_number = 0; $word_number < $size; ++$word_number)
        {
            $word_index = random_int(0, count($corpus_words) - 1);
            $word = $corpus_words[$word_index];
            $file_text = $file_text . $word . "\n";
        }
        file_put_contents($file_name, $file_text);
    }
}
?>
