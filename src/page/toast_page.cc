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
const PageOffset SLOT_DIR_OFFSET = 16;
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
    for(SlotID i = 0; i < _nSlots; i++)
    {
        Size size;
        PageOffset offset;
        memcpy((uint8_t*)&size, slots+i*4,2);
        memcpy((uint8_t*)&offset, slots+i*4+2, 2);
        slot_dir.push_back(std::pair<Size, PageOffset>(size, offset));
        slot_organizer[offset] = i;
    }
}
SlotID ToastPage::InsertRecord(const uint8_t *src, Size size){
    assert(Fits(size));
    for(SlotID i = 0; i < _nSlots; i++)
    {
        if(slot_dir[i].second == NULL_OFFSET)
        {
            SetData(src, size, free_offset);
            slot_dir[i].first = size;
            slot_dir[i].second = free_offset;
            free_offset += size;
            slot_organizer[free_offset] = i;
            return i;
        }

    }
    //all currently used slots full, create new slot
    _nSlots++;
    SetData(src, size, free_offset);
    slot_organizer[free_offset] = _nSlots - 1;
    slot_dir.push_back(std::pair<Size, PageOffset>(size, free_offset));
    free_offset += size;
    return _nSlots-1;
}
uint8_t *ToastPage::GetRecord(SlotID nSlotID){
    Size size = slot_dir[nSlotID].first;
    PageOffset offset = slot_dir[nSlotID].second;
    uint8_t *rec = new uint8_t[size];
    GetData(rec, size, offset);
    return rec;
}
bool ToastPage::HasRecord(SlotID nSlotID){
    if(nSlotID < _nSlots)
        return false;
    if(slot_dir[nSlotID].second == NULL_OFFSET)
        return false;
    return true;
}
void ToastPage::DeleteRecord(SlotID nSlotID){
    PageOffset end = slot_dir[nSlotID].second + slot_dir[nSlotID].first;
    PageOffset offset = slot_dir[nSlotID].second;
    slot_dir[nSlotID].second = NULL_OFFSET;
    auto it = slot_organizer.find(offset);
    auto previt = it;
    it++;
    slot_organizer.erase(previt);
    while(it != slot_organizer.end())
    {
        PageOffset newoffset = end - slot_dir[it->second].second;
        moveSlot(slot_dir[it->second], newoffset);
        slot_dir[it->second].second = newoffset;
        slot_organizer[newoffset] = it->second;
        end = newoffset;
        previt = it;
        it++;
        slot_organizer.erase(previt);

    }
    free_offset = end;
}
void ToastPage::moveSlot(Slot s, PageOffset newpos){
    uint8_t *data = new uint8_t[s.first];
    GetData(data, s.first, s.second);
    SetData(data, s.first, newpos);
}
void ToastPage::UpdateRecord(SlotID nSlotID, const uint8_t *src){
    if (slot_dir[nSlotID].second == NULL_OFFSET) throw RecordPageException(nSlotID);
    SetData(src, slot_dir[nSlotID].first, slot_dir[nSlotID].second);
}
void ToastPage::GetData(uint8_t *dst, PageOffset nSize, PageOffset nOffset) const{
    MiniOS::GetOS()->ReadPage(_nPageID, dst, nSize, END_OFFSET - nOffset);
}
void ToastPage::SetData(const uint8_t *src, PageOffset nSize, PageOffset nOffset){
    MiniOS::GetOS()->WritePage(_nPageID, src, nSize, END_OFFSET - nOffset);
    
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
    return (_nSlots + 1)*4 + SLOT_DIR_OFFSET >= END_OFFSET - free_offset;
}
void ToastPage::Clear(){
    for(SlotID i = 0; i < _nSlots; i++)
    {
        slot_dir[i].second = NULL_OFFSET;
    }
}


}
