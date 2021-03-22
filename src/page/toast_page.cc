#include <assert.h>

#include <cstring>

#include "exception/exceptions.h"
#include "macros.h"
#include "minios/os.h"
#include "defines.h"
#include "toast_page.h"

namespace thdb {
const PageOffset SLOT_NUM_OFFSET = 12;
const PageOffset SLOT_FREESPACE_OFFSET= 14;
const PageOffset SLOT_DIR_OFFSET;
const PageOffset END_OFFSET = 4096;
const PageOffset NULL_OFFSET = 4096;
ToastPage::ToastPage(uint16_t nSlots, bool):LinkedPage(){
    _nSlots = nSlots;
    free_offset = 0;
    SetHeader((uint8_t *)&_nSlots, 2, SLOT_NUM_OFFSET);
    SetHeader((uint8_t*)&free_offset, 2, SLOT_FREESPACE_OFFSET);
}

ToastPage::ToastPage(PageID nPageID):LinkedPage(nPageID){
    GetHeader((uint8_t *)&_nSlots, 2, SLOT_NUM_OFFSET);
    GetHeader((uint8_t*)&free_offset, 2, SLOT_FREESPACE_OFFSET);
    SetSlotDir();
}
ToastPage::~ToastPage() { 
    
}
void ToastPage::SetSlotDir(){
    uint8_t *slots;
    GetHeader(slots, _nSlots * 4, SLOT_DIR_OFFSET);
    for(uint16_t i = 0; i < _nSlots; i++)
    {
        Size size;
        PageOffset offset;
        memcpy((uint8_t*)&size, slots+i*4,2);
        memcpy((uint8_t*)&offset, slots+i*4+2, 2);
        slot_dir.push_back(std::pair<Size, PageOffset>(size, offset));
    }
}
SlotID ToastPage::InsertRecord(const uint8_t *src, Size size){
    assert(Fits(size));
    for(SlotID i = 0; i < _nSlots; i++)
    {
        if(slot_dir[i].second == NULL_OFFSET)
        {

        }

    }
}
uint8_t *ToastPage::GetRecord(SlotID nSlotID){

}
void ToastPage::DeleteRecord(SlotID nSlotID){

}
void ToastPage::UpdateRecord(SlotID nSlotID, const uint8_t *src){

}
void ToastPage::GetData(uint8_t *dst, PageOffset nSize, PageOffset nOffset) const{
    //MiniOS::GetOS()->ReadPage(_nPageID, dst, nSize, nOffset + DATA_BEGIN_OFFSET);
}
void ToastPage::SetData(const uint8_t *src, PageOffset nSize, PageOffset nOffset){
    //MiniOS::GetOS()->WritePage(_nPageID, src, nSize, nOffset + DATA_BEGIN_OFFSET);
    
}
Size ToastPage::GetUsed() const{

}
bool ToastPage::Fits(Size size) const{
    if((_nSlots + 1)*4 + SLOT_DIR_OFFSET >= END_OFFSET - free_offset- size)
    {
        return false;
    }
    return true;
}
bool ToastPage::Full() const{

}
void ToastPage::Clear(){

}


}
