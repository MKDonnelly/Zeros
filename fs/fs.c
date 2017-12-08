#include "fs.h"

fs_node_t *fs_root = 0;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer){
   if( node->read != 0 )
      return node->read(node, offset, size, buffer);
   else
      return 0;
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer){
   if( node->write != 0 )
      return node->write(node, offset, size, buffer);
   else
      return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write){
   if( node->open != 0 )
      node->open(node);
}

void close_fs(fs_node_t *node){
   if( node->close != 0 )
      node->close(node);
}


fs_node_t *readdir_fs(fs_node_t *node, uint32_t index){
   if( node->readdir != 0 && (node->flags & FS_DIRECTORY))
      return node->readdir(node, index);
   else
      return 0;
}

/*
struct dirent *readdir_fs(fs_node_t *node, uint32_t index){
   if( node->readdir != 0 && (node->flags & FS_DIRECTORY))
      return node->readdir(node, index);
   else
      return 0;
}*/

fs_node_t *finddir_fs(fs_node_t *node, char *name){
   if( node->finddir != 0 && (node->flags & FS_DIRECTORY))
      return node->finddir(node, name);
   else
      return 0;
}
