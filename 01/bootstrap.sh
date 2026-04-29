#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Error: Correct usage is $0 PLUGIN_NAME"
    exit 1
fi

PLUGIN_NAME=$1

if [ -d "$1" ]; then
    echo "Error: Directory $PLUGIN_NAME already exists."
    exit 2
fi

SCRIPT_DIR=$(dirname "$0")
INPUT_DIR="$SCRIPT_DIR/input"


mkdir -p "$PLUGIN_NAME" "$PLUGIN_NAME/src" "$PLUGIN_NAME/bin" "$PLUGIN_NAME/logs" "$PLUGIN_NAME/resources"

echo "Creating plugin $PLUGIN_NAME at $(date)" > "$PLUGIN_NAME/logs/setup.log"

sed "s/#PLUGIN_NAME#/$PLUGIN_NAME/g" "$INPUT_DIR/Makefile" > "$PLUGIN_NAME/Makefile"
sed "s/#PLUGIN_NAME#/$PLUGIN_NAME/g" "$INPUT_DIR/README" > "$PLUGIN_NAME/README"

for FILE in "$INPUT_DIR/src/"*; do
    FILENAME=$(basename "$FILE")
    sed "s/#PLUGIN_NAME#/$PLUGIN_NAME/g" "$FILE" > "$PLUGIN_NAME/src/$FILENAME" 
done

for FILE in "$INPUT_DIR/resources/"*; do
    FILENAME=$(basename "$FILE")
    sed "s/#PLUGIN_NAME#/$PLUGIN_NAME/g" "$FILE" > "$PLUGIN_NAME/resources/$FILENAME"
    if [[ "$FILENAME" == *.sh ]]; then
        FIRST_LINE=$(head -n 1 "$PLUGIN_NAME/resources/$FILENAME")
        if [ "$FIRST_LINE" = "#!/bin/bash" ]; then
            chmod 744 "$PLUGIN_NAME/resources/$FILENAME"
        else
            ERR_MSG="Error: resource script $FILENAME does not start with #!/bin/bash"
            echo "$ERR_MSG" >&2 
            echo "$ERR_MSG" >> "$PLUGIN_NAME/logs/setup.log"
        fi
    fi
done

echo "The following initial directory structure was created:" >> "$PLUGIN_NAME/logs/setup.log"
ls -R "$PLUGIN_NAME" >> "$PLUGIN_NAME/logs/setup.log"