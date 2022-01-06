CONST
	STDOUT_INPUT_FILE = 'Music file: %s';
	STDOUT_OUTPUT_FILE = 'Primary output file: %s';
	STDOUT_REDUCE_SET = 'Reducing: %s';
	STDOUT_REINDEX_SET = 'Reindexing: %s';
	STDOUT_SFX_ONLY = 'only SFX';
	STDOUT_TAB_ONLY = 'only TAB';
	STDOUT_ALL = 'all';
	STDOUT_UNRECOGNIZED_OPTION = 'Unrecognized `%s` option.';

	STDOUT_ORIGIN_ADDRESS = 'Origin address: %.4x';
	STDOUT_REDUCE_SFX_LIST = 'SFX used: ';
	STDOUT_REDUCE_SFX_ID = '#%d ';
	STDOUT_REDUCE_SFX_SUMMARY = 'Summary: %d SFX(s) is used.';
	STDOUT_REDUCE_TAB_LIST = 'TAB used: ';
	STDOUT_REDUCE_TAB_ID = '#%d ';
	STDOUT_REDUCE_TAB_SUMMARY = 'Summary: %d TAB(s) is used.';
	ERR_SFX_REINDEX = 'Something goes wrong.';

	ERR_BAD_ADDRESS = 'Warning: address was not specified or it''s wrong.'+EOL;
	ERR_BAD_ADDRESS_DEFAULT_SET = ERR_BAD_ADDRESS+'%s is set to default ($%x)';
	ERR_BAD_ADDRESS_ORIGIN_CONTINUE = ERR_BAD_ADDRESS+'%s continue origin';

	STDOUT_ORIGIN = 'Origin';
	STDOUT_AUDIO_BUFFER = 'Audio buffer';
	STDOUT_SFX_REGISTERS = 'SFX registers';
	STDOUT_SFX_CHANNELS_REGS ='SFX channels registers';
	STDOUT_NOTE_TABLE_DATA = 'Note table data';
	STDOUT_TABLE_OF_SFX_NOTE = 'Table of SFX note';
	STDOUT_TABLE_OF_SFX_MODE = 'Table of SFX mode';
	STDOUT_TABLE_OF_SFX_PTRS = 'Table of SFX pointers';
	STDOUT_TABLE_OF_TAB_PTRS = 'Table of TAB pointers';
	STDOUT_SONG_DATA = 'Song data';
	STDOUT_SFXTAB_DATA = 'SFX/TAB data';

	STDOUT_READING_SMM_FILE = 'Reading %s file...';
	STDOUT_SUMMARY_SFX = 'Total SFXs defined: %d';
	STDOUT_SUMMARY_TAB = 'Total TABs defined: %d';
	STDOUT_SUMMARY_DATA_SIZE = 'Data (SFXs & TABs) size: %d byte(s)';

	STDOUT_ADDRESS_SET = '%s set: $%.*x';
	STDOUT_OUTPUT_FILENAME = ' output to `%s` file';
	STDOUT_OUTPUT_FILENAME_SET = ' output is set to `%s` file';
	STDOUT_PAGE_ALIGN = ' (page align)';

  STDOUT_CREATE_FILE = 'Create file %s...';
	STDOUT_CREATE_DIRECTORY = 'Create directory %s...';
  STDOUT_CREATE_DIRECTORY_OK = 'OK';

	ERR_FAILED_CREATE_DIR = 'Failed to create directory';