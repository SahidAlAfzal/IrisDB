#ifndef B_PLUS_TREE_PAGE_H
#define B_PLUS_TREE_PAGE_H

enum class IndexPageType { INVALID_INDEX_PAGE = 0, LEAF_PAGE, INTERNAL_PAGE};

class BPlusTreePage {
private:
    IndexPageType page_type_;
    int lsn_;
    int size_;
    int max_size_;
    int parent_page_id_;
    int page_id_;

    public:
    // We will only need getters and setters here!
    IndexPageType GetPageType() const;
    void SetPageType(IndexPageType page_type);

    int GetSize() const;
    void SetSize(int size);
    void IncreaseSize(int amount);

    int GetMaxSize() const;
    void SetMaxSize(int max_size);

    int GetParentPageId() const;
    void SetParentPageId(int parent_page_id);

    int GetPageId() const;
    void SetPageId(int page_id);
};

#endif