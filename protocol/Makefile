all: fk-atlas.proto.json fk-atlas.pb.go src/fk-atlas.pb.c src/fk-atlas.pb.h

node_modules/.bin/pbjs:
	npm install

fk-atlas.proto.json: node_modules/.bin/pbjs fk-atlas.proto
	pbjs fk-atlas.proto -t json -o fk-atlas.proto.json

src/fk-atlas.pb.c src/fk-atlas.pb.h: fk-atlas.proto
	protoc --nanopb_out=./src fk-atlas.proto

fk-atlas.pb.go: fk-atlas.proto
	protoc --go_out=./ fk-atlas.proto

clean:

veryclean:
