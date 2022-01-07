class DrawableContainer
{
protected:
    std::shared_ptr<Shape> drawable;
    std::shared_ptr<Animation> animation;

public:
    DrawableContainer(std::shared_ptr<Shape> shape) noexcept;
    virtual ~DrawableContainer() noexcept = default;

    virtual void draw();

    virtual bool hasAnimation();
    virtual void addAnimation(std::shared_ptr<Animation> newanim);
    virtual void removeAnimation();
    virtual void animationFinished(AnimationT);
};
