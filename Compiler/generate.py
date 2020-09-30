TOKENS_FILENAME = "GeneratedRules.txt"

SPACES_COUNT = 0
TOKEN_PREFIX = ""
KEYWORD_SUFFIX = " { PrintKeyword(yytext); } "
TOKEN_SUFFIX = "\n"

FILE_HEADER = ( 
    # "#pragma once\n" +
    # "enum Tokens {\n" +
    # (' ' * SPACES_COUNT) + TOKEN_PREFIX + "String" + TOKEN_SUFFIX +
    # (' ' * SPACES_COUNT) + TOKEN_PREFIX + "VerbatimString" + TOKEN_SUFFIX 
)
# FILE_FOOTER = "};\n"

tokens_file_content = ""

with open("keywords.txt", 'r') as keywords_file:
    for keyword in sorted(keywords_file):
        keyword_token_str = (' ' * SPACES_COUNT) + TOKEN_PREFIX + keyword.strip() + KEYWORD_SUFFIX + TOKEN_SUFFIX
        tokens_file_content += keyword_token_str

# tokens_file_content += FILE_FOOTER

with open(TOKENS_FILENAME, 'w') as tokens_file:
    tokens_file.write(tokens_file_content)
    