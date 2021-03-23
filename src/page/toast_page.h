#ifndef THDB_TOAST_PAGE_H_
#define THDB_TOAST_PAGE_H_

#include "page/linked_page.h"
#include "utils/bitmap.h"
#include "defines.h"
namespace thdb {
typedef std::pair<Size, PageOffset> Slot; 
/**
 * @brief 变长记录页面。
 *
 */
class ToastPage : public LinkedPage {
 public:
  ToastPage(uint16_t nSlots, bool);
  ToastPage(PageID nPageID);
  ~ToastPage();

  /**
   * @brief 插入一条定长记录
   *
   * @param src 记录定长格式化后的内容
   * @return SlotID 插入位置的槽编号
   */
  SlotID InsertRecord(const uint8_t *src, Size size);
  /**
   * @brief 获取指定位置的记录的内容
   *
   * @param nSlotID 槽编号
   * @return uint8_t* 记录定长格式化的内容
   */
  uint8_t *GetRecord(SlotID nSlotID);
  /**
   * @brief 判断某一个槽是否存在记录
   *
   * @param nSlotID 槽编号
   * @return true 存在记录
   * @return false 不存在记录
   */
  bool HasRecord(SlotID nSlotID);
  /**
   * @brief 删除制定位置的记录
   *
   * @param nSlotID 槽编号
   */
  void DeleteRecord(SlotID nSlotID);
  /**
   * @brief 原地更新一条记录的内容
   *
   * @param nSlotID 槽编号
   * @param src 新的定长格式化内容
   */
  void UpdateRecord(SlotID nSlotID, const uint8_t *src);
  /**
   * @brief 读出无格式页面数据部分的内容。
   * 
   * @param dst 读出内容存放地址
   * @param nSize 读出内容长度
   * @param nOffset 读出内容在数据部分起始位置
   */
  void GetData(uint8_t *dst, PageOffset nSize, PageOffset nOffset) const;
  /**
   * @brief 写入无格式页面数据部分的内容。
   * 
   * @param src 写入内容存放地址
   * @param nSize 写入内容长度
   * @param nOffset 读出内容在数据部分起始位置
   */
  void SetData(const uint8_t *src, PageOffset nSize, PageOffset nOffset);
  
  //Size GetCap() const;
  Size GetUsed() const;
  bool Fits(Size size) const;
  bool Full() const;
  void Clear();
 private:
  void SetSlotDir();
  void moveSlot(Slot s, PageOffset newpos);
//   void StoreBitmap();
//   void LoadBitmap();
  /**
   * number of slots in page
   */
  uint16_t _nSlots;
  PageOffset free_offset;
  /**
   * @brief 记录槽的大小和offset
   */
  std::vector<Slot> slot_dir;

  std::map<PageOffset, SlotID> slot_organizer;
  /**
   * @brief 表示页面能容纳的记录数量
   */
  Size _nCap;

};

}  // namespace thdb

#endif